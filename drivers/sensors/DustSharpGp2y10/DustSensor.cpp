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
		m_u16AdcResultSum(0)
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

		return (ET_OK);
	}

	ERRORTYPE DustSensor::Run()
	{
		ERRORTYPE eRet = ET_OK;

		uint8_t u8Loop = 0;
		m_u16AdcResultSum = 0;
		uint16_t u16Temp = 0;
		while(u8Loop < MEASUREMENT_COUNT)
		{
			u16Temp = makeSingleMeas();

			_delay_us(SLEEP_TIMEOUT_US);

			if(MIN_ADC_VOLTAGE_MV < u16Temp)
			{
				m_u16AdcResultSum += u16Temp;
			}

			++u8Loop;
		}

		m_u16AdcResultSum = (m_u16AdcResultSum/MEASUREMENT_COUNT);

		if(MIN_ADC_VOLTAGE_MV > m_u16AdcResultSum)
		{
			m_u16DustQuantity = 0;
		}
		else
		{
			m_u16DustQuantity =  ((17U * (m_u16AdcResultSum)) - 10000U)/100U; // precise function approximation
		}

		return eRet;
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

} /* namespace sensors */
} /* namespace drivers */
