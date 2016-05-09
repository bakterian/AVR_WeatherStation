/*
 * DustSensor.cpp
 *
 *  Created on: 15.04.2016
 *      Author: Marcin.Karczewski
 */

#include "DustSensor.h"
#include <avr/io.h>
#include "stdint.h"
#include <util/delay.h>

namespace drivers
{
namespace sensors
{

	DustSensor::DustSensor(const ISensor::Configuration& rConfig):
		ISensor(rConfig),
		m_u16DustQuantity(0),
		m_u16AdcResultSum(0),
		m_u8MeasurementLoop(0),
		m_u8SensorState(STATE_IDLE),
		m_u8NewSensorState(STATE_IDLE),
		m_sMeasTimeout(MEAS_TICK_TIMEOUT)
	{

	}

	DustSensor::~DustSensor()
	{

	}

	ERRORTYPE DustSensor::initialize()
	{
		/* ADC SETTINGS */
		ADCSRA = (1<<ADEN) 					// ADEN=1 turning the ADC on
		         |(1<<ADPS0) 				// setting the prescaller to 128 (with a 8 MHz F_CPU this will result in a sampling rate of 62 500 Hz)
		         |(1<<ADPS1)
		         |(1<<ADPS2);

		ADMUX  = (1<<REFS0) |(1<<REFS1) ;	// REFS0=1,REFS1=1 2,56 with external capacitor at AREF pin
											// Performing a measurement in the single ended mode on channel ADC0
		DDRA &= ~_BV(DDA0);
		PINA &= ~_BV(PINA0);

		/* SENSOR LED SETTINGS */
		DDRB  |= (1 << DDB0);				// setting pin B0 as output
		PORTB |= _BV(SENOR_INTERNAL_LED);	// output B0 set low

		vTaskSetTimeOutState(&m_sMeasTimestamp);

		m_u8NewSensorState = STATE_STAND_BY;

		return (ET_OK);
	}

	ERRORTYPE DustSensor::run()
	{
		ERRORTYPE eRet = ET_OK;

		if(m_u8NewSensorState != m_u8SensorState)
		{
			/*
			if(m_u8NewSensorState == STATE_RESULT_CALCUALATION )
			{
				xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
				xSerialxPrintf_P( &xSerialPort, PSTR("/ %s / New result is ready, current system tick: %d.\r\n"), this->getDescription(),xTaskGetTickCount());
				xSemaphoreGive(xConsoleMutex);
			}
			*/

			m_u8SensorState = m_u8NewSensorState;
		}

		switch(m_u8NewSensorState)
		{
			case STATE_STAND_BY:
				this->doResetOfMeasurementData();
				break;

			case STATE_MEASURING:
				this->doMeasuring();
				break;

			case STATE_RESULT_CALCUALATION:
				this->doDustCalculation();
				break;

			case STATE_ERROR:
				this->doErrorStatusPrint();
				eRet = ET_NOK;
				break;

			default:
				break;
		}

		return (eRet);
	}

	uint32_t DustSensor::getResult()
	{
		return (uint32_t) m_u16DustQuantity;
	}

	uint16_t DustSensor::makeSingleMeas()
	{
		uint16_t u16Ret = 0;
		setLED();
		_delay_us(PRE_SAMPLING_TIMEOUT_US);
		u16Ret = readAdcValue();
		_delay_us(POST_SAMPLING_TIMEOUT_US);
		clearLED();

		return u16Ret;
	}

	uint16_t DustSensor::readAdcValue()
	{
	    ADCSRA |= (1<<ADSC); 		//ADSC: enabling one single conversion

	    while(ADCSRA & (1<<ADSC)); 	//waiting for the measurement to end

	    return (uint16_t)(((ADC*ADC_VREF_MV)/ADC_RESOLUTION) * (uint16_t)11U);
	}

	void DustSensor::setLED()
	{
		PORTB &= ~_BV(SENOR_INTERNAL_LED);
	}

	void DustSensor::clearLED()
	{
		PORTB |= _BV(SENOR_INTERNAL_LED);
	}

	void DustSensor::doResetOfMeasurementData()
	{
		m_u8MeasurementLoop = 0;
		m_u16AdcResultSum = 0;

		m_u8NewSensorState = STATE_MEASURING;
	}

	void DustSensor::doMeasuring()
	{
		if( pdTRUE == xTaskCheckForTimeOut(&m_sMeasTimestamp, &m_sMeasTimeout))
		{
			uint16_t u16Temp = 0;

			u16Temp = makeSingleMeas();

			if(MIN_ADC_VOLTAGE_MV < u16Temp)
			{
				m_u16AdcResultSum += u16Temp;
			}

			++m_u8MeasurementLoop;

			if(m_u8MeasurementLoop >= MEASUREMENT_COUNT)
			{
				m_u8NewSensorState = STATE_RESULT_CALCUALATION;
			}

			//next measurement will start no sooner than after 10 ms.
			vTaskSetTimeOutState(&m_sMeasTimestamp);
			m_sMeasTimeout = MEAS_TICK_TIMEOUT;
		}

	}

	void DustSensor::doDustCalculation()
	{
		m_u16AdcResultSum = (m_u16AdcResultSum/MEASUREMENT_COUNT);

		if(MIN_ADC_VOLTAGE_MV > m_u16AdcResultSum)
		{
			m_u16DustQuantity = 0;
		}
		else
		{
			m_u16DustQuantity =  ((17U * (m_u16AdcResultSum)) - 10000U)/100U; // precise function approximation
		}

		m_u8NewSensorState = STATE_STAND_BY;
	}

	void DustSensor::doErrorStatusPrint()
	{
		xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
		xSerialxPrintf_P( &xSerialPort, PSTR("/ %s / Entered Error State something went horribly wrong! "), this->getDescription());
		xSemaphoreGive(xConsoleMutex);

		m_u8NewSensorState = STATE_IDLE;
	}

} /* namespace sensors */
} /* namespace drivers */
