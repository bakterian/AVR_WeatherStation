/*
 * TempHumiditySensor.cpp
 *
 *  Created on: 18.04.2016
 *      Author: Marcin.Karczewski
 */
#include "TempHumiditySensor.h"

namespace drivers
{
namespace sensors
{
	TempHumiditySensor::TempHumiditySensor(const DhtConfiguartion& rConfig):
	ISensor(rConfig.sBaseConfig),
	m_sDhtConfig(rConfig)
	{

	}

	TempHumiditySensor::~TempHumiditySensor()
	{

	}

	ERRORTYPE TempHumiditySensor::initialize()
	{
		ERRORTYPE eRet = m_sDhtConfig.pDhtTalker->init();
		return (eRet);
	}

	ERRORTYPE TempHumiditySensor::run()
	{
		if((m_sDhtConfig.pDhtTalker != NULL))
		{//perform serial measurement only if the dht talker was provided and this a humidity sensor instance
			m_sDhtConfig.pDhtTalker->run();
		}
		return (ET_OK);
	}

	uint32_t TempHumiditySensor::getResult(MeasDataType eMeasDataType)
	{
		uint32_t u32Ret = 0xFFFFFFFFU;

		uint8_t* pu8ReceivedData = m_sDhtConfig.pDhtTalker->getReceivedData();

		m_sDhtConfig.sDhtMeasurementData.u16Humidity = (((pu8ReceivedData[0U] << 8U) + pu8ReceivedData[1U])/ 10U);
		m_sDhtConfig.sDhtMeasurementData.u16Temperature = (((pu8ReceivedData[2U] << 8U) + pu8ReceivedData[3U])/ 10U);
		m_sDhtConfig.sDhtMeasurementData.u8Crc = pu8ReceivedData[4U];

		if(eMeasDataType == Humidity)
		{
			u32Ret = m_sDhtConfig.sDhtMeasurementData.u16Humidity;
		}
		else if(eMeasDataType == Temperature)
		{
			u32Ret = m_sDhtConfig.sDhtMeasurementData.u16Temperature;
		}

		return (u32Ret);
	}

} /* namespace sensors */
} /* namespace drivers */
