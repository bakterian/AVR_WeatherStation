/*
 * ISensor.cpp
 *
 *  Created on: 15.04.2016
 *      Author: Marcin.Karczewski
 */

#include "ISensor.h"

namespace drivers
{
namespace sensors
{

ISensor::ISensor(const Configuration& rConfig):
m_ISensorConfig(rConfig)
{

}

ISensor::~ISensor()
{

}

ERRORTYPE ISensor::initialize()
{
	return (ET_OK);
}

uint8_t ISensor::getSensorState() const
{
	return uint8_t(0xFFU);
}

ERRORTYPE ISensor::run()
{
	return (ET_NOK);
}

PGM_P ISensor::getDescription() const
{
	return m_ISensorConfig.psSensorDescription;
}

PGM_P ISensor::getSensorUnits() const
{
	return m_ISensorConfig.psSensorUnits;
}

SensorType ISensor::getSensorType() const
{
	return m_ISensorConfig.eSensorType;
}

} /* namespace sensors */
} /* namespace drivers */
