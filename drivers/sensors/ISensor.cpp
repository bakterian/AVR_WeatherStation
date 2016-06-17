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

uint32_t ISensor::getResult(MeasDataType eMeasDataType)
{
	return 0xFFFFFFFF;
}

uint8_t ISensor::getSensorState() const
{
	return uint8_t(0xFFU);
}

ERRORTYPE ISensor::run()
{
	return (ET_NOK);
}

ISensor::MeasurementDataInfo ISensor::getMeasDataInfo(uint8_t u8MeasDataNo) const
{
	return m_ISensorConfig.pMeasurmentDataList[u8MeasDataNo];
}

uint8_t ISensor::getMeasDataCount() const
{
	return m_ISensorConfig.u8MeasurmentDataCount;
}

} /* namespace sensors */
} /* namespace drivers */
