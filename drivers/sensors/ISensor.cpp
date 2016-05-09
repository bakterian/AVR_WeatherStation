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

ERRORTYPE ISensor::run()
{
	return (ET_NOK);
}

const uint8_t* ISensor::getDescription() const
{
	return m_ISensorConfig.csSensorDescription;
}

const uint8_t* ISensor::getSensorUnits() const
{
	return m_ISensorConfig.csSensorUnits;
}

SensorType ISensor::getSensorType() const
{
	return m_ISensorConfig.eSensorType;
}

} /* namespace sensors */
} /* namespace drivers */
