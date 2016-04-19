/*
 * SensorManager.cpp
 *
 *  Created on: 17.04.2016
 *      Author: Marcin.Karczewski
 */

#include "SensorManager.h"

namespace Application
{
namespace SensorManagement
{

	SensorManager::SensorManager(const SensorConfiguration& rConfig):
		m_sConfig(rConfig)
	{

	}


	SensorManager::~SensorManager()
	{

	}

	ERRORTYPE SensorManager::InitSensors()
	{
		ERRORTYPE eRet = ET_NOK;

		for(uint8_t u8Loop = 0; u8Loop < m_sConfig.u8SensorCount; ++u8Loop)
		{
			eRet = m_sConfig.pSensorList[u8Loop].initialize();

			if(ET_OK != eRet)
			{
				break;
			}
		}

		return eRet;
	}

	ERRORTYPE SensorManager::Run(xComPortHandle& rConsole, QueueHandle_t& rConsoleMutex)
	{
		ERRORTYPE eRet =  ET_OK;

		for(uint8_t u8Loop = 0; u8Loop < m_sConfig.u8SensorCount; ++u8Loop)
		{
			eRet = m_sConfig.pSensorList[u8Loop].Run();

			if(ET_OK != eRet)
			{
				break;
			}
			else
			{
				uint32_t u32Result = m_sConfig.pSensorList[u8Loop].getResult();
				xSemaphoreTake(rConsoleMutex, portMAX_DELAY);
				xSerialxPrintf_P( &xSerialPort, PSTR("%s = %d "), m_sConfig.pSensorList[u8Loop].getDescription(),u32Result);
				xSerialxPrintf_P( &xSerialPort, PSTR("%s\r\n"), m_sConfig.pSensorList[u8Loop].getSensorUnits());
				xSemaphoreGive(rConsoleMutex);
			}
		}

		return eRet;
	}

} /* namespace SensorManagement */
} /* namespace Application */
