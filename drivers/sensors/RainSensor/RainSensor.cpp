/*
 * RainSensor.cpp
 *
 *  Created on: 18.06.2016
 *      Author: Marcin.Karczewski
 */

#include "RainSensor.h"

namespace drivers
{
namespace sensors
{

RainSensor::RainSensor(const RainConfiguration& rConfig):
		ISensor(rConfig.sBaseConfig),
		m_sRainConfig(rConfig),
		m_u32LastResult(0xFFFFFFFFU),
		m_u8ReCaibrationTimer(0U),
		m_u32MaxVoltageLevel(RAIN_SENOR_MAX_VALUE)
{

}

RainSensor::~RainSensor()
{

}

ERRORTYPE RainSensor::initialize()
{
	return (ET_OK);
}

ERRORTYPE RainSensor::run()
{
	if(m_u8ReCaibrationTimer == uint8_t(0U))
	{//it'time measure the power supply voltage level
		m_u32MaxVoltageLevel = getAverageAdcValue(m_sRainConfig.u8CalibAdcChannel);
	}
	else
	{//do raindrop procentage measurment
		uint32_t u32Result = getAverageAdcValue(m_sRainConfig.u8AdcChannel);

		//caluclate the avarage and express the result in proccent of the max voltage
		m_u32LastResult = uint32_t(100U) - ((uint32_t(100U) * u32Result) / m_u32MaxVoltageLevel);
	}

	m_u8ReCaibrationTimer = (m_u8ReCaibrationTimer + 1) % 100;

	return (ET_OK);
}

uint32_t RainSensor::getAverageAdcValue(uint8_t u8AdcChannelId)
{
	uint32_t m_u32TempResult = 0;
	m_sRainConfig.pAdcManager->intialize(u8AdcChannelId);

	for( uint8_t u8Loop = 0; u8Loop < RAIN_SENSOR_SAMPLE_COUNT; ++u8Loop)
	{
		m_u32TempResult += m_sRainConfig.pAdcManager->readAdcValue(u8AdcChannelId);
	}

	return (m_u32TempResult / RAIN_SENSOR_SAMPLE_COUNT);
}

uint32_t RainSensor::getResult(MeasDataType eMeasDataType)
{
	return m_u32LastResult;
}

} /* namespace sensors */
} /* namespace drivers */
