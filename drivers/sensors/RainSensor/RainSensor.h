/*
 * RainSensor.h
 *
 *  Created on: 18.06.2016
 *      Author: Marcin.Karczewski
 */

#ifndef DRIVERS_SENSORS_RAINSENSOR_RAINSENSOR_H_
#define DRIVERS_SENSORS_RAINSENSOR_RAINSENSOR_H_

#include "..\ISensor.h"
#include "..\..\..\Configuration\Globals.h"
#include "..\..\Utils\adc\AdcManager.h"

namespace drivers
{
namespace sensors
{

class RainSensor : public ISensor
{
public:
	#define RAIN_SENOR_MAX_VALUE				uint16_t(2500)
	#define RAIN_SENSOR_SAMPLE_COUNT 			uint8_t(10U)

	struct RainConfiguration
	{
		::drivers::sensors::AdcManager*			pAdcManager;		// Pointer to the Adc Manager
		uint8_t									u8AdcChannel;		// Specifies the Adc Channel which will be used for the measurement
		uint8_t									u8CalibAdcChannel;	// Specifies the Adc Channel which will be used for calibration
		ISensor::Configuration	 				sBaseConfig;		// ISensor base configuration
	};

	/**
	 * \CTOR
	 */
	RainSensor(const RainConfiguration& rConfig);

	/**
	 * \DTOR
	 */
	virtual ~RainSensor();

	/**
	 * \brief triggers sensor initialization
	 * \return ET_OK - sensor was initialized
	 */
	virtual ERRORTYPE initialize();

	/**
	 * \brief cyclicly triggers measurement start
	 * \return ET_OK - measurement finished without problems
	 * \return ET_OK_NOT - measurement finished with problems
	 * \return ET_PENDING - measurement is ongoing
	 */
	virtual ERRORTYPE run();

	/**
	 * \brief delivers the measurement result.
	 * \return sensor measurement result
	 */
	virtual uint32_t getResult(MeasDataType eMeasDataType);

private:
	/**
	 * \brief calculates an avarage voltage value of the chosen adc channel
	 * \return u8AdcChannelId - adc channel number
	 * \return average voltage value of the chosen adc channel
	 */
	uint32_t		getAverageAdcValue(uint8_t u8AdcChannelId);

	RainConfiguration						m_sRainConfig;
	uint32_t								m_u32LastResult;
	uint8_t									m_u8ReCaibrationTimer;
	uint16_t								m_u32MaxVoltageLevel;
};

} /* namespace sensors */
} /* namespace drivers */

#endif /* DRIVERS_SENSORS_RAINSENSOR_RAINSENSOR_H_ */
