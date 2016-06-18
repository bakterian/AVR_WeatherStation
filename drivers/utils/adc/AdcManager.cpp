/*
 * AdcManager.cpp
 *
 *  Created on: 18.06.2016
 *      Author: Marcin.Karczewski
 */

#include "AdcManager.h"

namespace drivers
{
namespace sensors
{

	AdcManager::AdcManager(const Configuration& rConfig):
		m_sConfig(rConfig)
	{

	}

	AdcManager::~AdcManager()
	{

	}

	void AdcManager::intialize(uint8_t u8AdcChannelId)
	{
		//TODO: initialization should be dependent on the config
		ADC_DDR_REG &= ~_BV(u8AdcChannelId);
		ADC_PIN_REG &= ~_BV(u8AdcChannelId);

		ADCSRA = (1<<ADEN) 					// ADEN=1 turning the ADC on
		         |(1<<ADPS0) 				// setting the prescaller to 128 (with a 8 MHz F_CPU this will result in a sampling rate of 62 500 Hz)
		         |(1<<ADPS1)
		         |(1<<ADPS2);

		ADMUX  = (1<<REFS0) |(1<<REFS1);	// REFS0=1,REFS1=1 2,56 with external capacitor at AREF pin
											// Performing a measurement in the single ended mode on channel u8AdcChannelId

		ADMUX  |= u8AdcChannelId;
	}

	uint16_t AdcManager::readAdcValue(uint8_t u8AdcChannelId)
	{
	    ADCSRA |= (1<<ADSC); 		//ADSC: enabling one single conversion

	    while(ADCSRA & (1<<ADSC)); 	//waiting for the measurement to end

	    return (uint16_t)(((ADC*ADC_VREF_MV)/(ADC_RESOLUTION << (uint8_t)m_sConfig.eAdcResoultion)));
	}

} /* namespace sensors */
} /* namespace drivers */
