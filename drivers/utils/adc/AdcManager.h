/*
 * AdcManager.h
 *
 *  Created on: 18.06.2016
 *      Author: Marcin.Karczewski
 */

#ifndef DRIVERS_UTILS_ADC_ADCMANAGER_H_
#define DRIVERS_UTILS_ADC_ADCMANAGER_H_

#include "..\..\..\Configuration\Globals.h"

namespace drivers
{
namespace sensors
{

#define ADC_VREF_MV            		2560Ul
#define ADC_RESOLUTION		   		1024Ul

#ifdef __AVR_ATmega32__
#define ADC_PIN_REG			PINA
#define	ADC_DDR_REG			DDRA
#define ADC_Channel_0 		PA0
#define ADC_Channel_1 		PA1
#define ADC_Channel_2 		PA2
#define ADC_Channel_3 		PA3
#define ADC_Channel_4 		PA4
#define ADC_Channel_5 		PA5
#define ADC_Channel_6 		PA6
#define ADC_Channel_7		 PA7
#else
	#warning "Update adc channel list for AVR mcu!"
#endif


/**
 * \Classs TwiManager
 * Provides a interface for accessing the adc input data
 */
class AdcManager
{
public:
	enum AdcVoltageReference
	{
		AREF_Pin 		= 	0,
		AVCC_Pin 		= 	1,
		Internal_2_56V 	= 	3
	};

	enum AdcOperationMode
	{
		SingleEndedMode,
		DifferentialMode
	};

	enum AdcResoulution
	{
		Normal_10bit,
		Oversampled_11bit,
		Oversampled_12bit
	};

	struct Configuration
	{
		AdcVoltageReference		eVoltagaRefSource;			// defines the voltage reference to be used during measurment
		AdcOperationMode		eAdcOperationMode;			// defines the adc operation mode
		AdcResoulution			eAdcResoultion;				// defines the adc resolution, software oversampling is supported
	};

	/**
	 * \brief CTOR
	 */
	AdcManager(const Configuration& rConfig);

	/**
	 * \brief DTOR
	 */
	virtual ~AdcManager();

	/**
	 * \brief initializes the adc manager by setting the registers accoridngly to the configuration
	 * \param u8AdcChannelId - the adc channel number
	 */
	void intialize(uint8_t u8AdcChannelId);

	/**
	 * brief Read the adc value.
	 * \param u8AdcChannelId - the adc channel number
	 * returns the rescaled voltage value in milivolt
	 */
	uint16_t readAdcValue(uint8_t u8AdcChannelId);

private:
	Configuration		m_sConfig;
};

} /* namespace sensors */
} /* namespace drivers */

#endif /* DRIVERS_UTILS_ADC_ADCMANAGER_H_ */
