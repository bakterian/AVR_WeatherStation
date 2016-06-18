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

	DustSensor::DustSensor(const Configuration& rConfig):
		ISensor(rConfig.sBaseConfig),
		m_u16DustQuantity(0),
		m_u16AdcResultSum(0),
		m_u8MeasurementLoop(0),
		m_u8SensorState(STATE_IDLE),
		m_u8NewSensorState(STATE_IDLE),
		m_sMeasTimeout(MEAS_TICK_TIMEOUT),
		m_pAdcManager(rConfig.pAdcManager),
		m_u8AdcChannel(rConfig.u8AdcChannel)
	{

	}

	DustSensor::~DustSensor()
	{

	}

	ERRORTYPE DustSensor::initialize()
	{
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

	uint32_t DustSensor::getResult(MeasDataType eMeasDataType)
	{
		return (uint32_t) m_u16DustQuantity;
	}

	uint16_t DustSensor::makeSingleMeas()
	{
		uint16_t u16Ret = 0;
		m_pAdcManager->intialize(m_u8AdcChannel);
		setLED();
		_delay_us(PRE_SAMPLING_TIMEOUT_US);
		u16Ret = m_pAdcManager->readAdcValue(m_u8AdcChannel) * (uint16_t)11U;
		_delay_us(POST_SAMPLING_TIMEOUT_US);
		clearLED();
		return u16Ret;
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
			vTaskSuspendAll();
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
			xTaskResumeAll();
		}

	}

	void DustSensor::doDustCalculation()
	{
		vTaskSuspendAll();
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
		xTaskResumeAll();
	}

	void DustSensor::doErrorStatusPrint()
	{
		xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
		xSerialxPrintf_P( &xSerialPort, PSTR("/ Dust Sensor / Entered Error State something went horribly wrong! "));
		xSemaphoreGive(xConsoleMutex);

		m_u8NewSensorState = STATE_IDLE;
	}

} /* namespace sensors */
} /* namespace drivers */
