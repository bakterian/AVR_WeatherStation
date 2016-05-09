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
		m_sConfig(rConfig),
		m_sLastResultPrintTimestamp(),
		m_sPrintResultsTimeout(m_sConfig.sPrintResultsTimeout)
	{

	}


	SensorManager::~SensorManager()
	{

	}

	ERRORTYPE SensorManager::initSensors()
	{
		ERRORTYPE eRet = ET_NOK;

		for(uint8_t u8Loop = 0; u8Loop < m_sConfig.u8SensorCount; ++u8Loop)
		{
			eRet = m_sConfig.pSensorList[u8Loop]->initialize();

			if(ET_OK != eRet)
			{
				break;
			}
		}

		vTaskSetTimeOutState(&m_sLastResultPrintTimestamp);

		return eRet;
	}

	ERRORTYPE SensorManager::run()
	{
		ERRORTYPE eRet =  ET_OK;

		for(uint8_t u8Loop = 0; u8Loop < m_sConfig.u8SensorCount; ++u8Loop)
		{
			eRet = m_sConfig.pSensorList[u8Loop]->run();

			if(ET_OK != eRet)
			{
				xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
				xSerialxPrintf_P( &xSerialPort, PSTR("/SENSOR MGR/ One of the sensors encountered problems."));
				xSemaphoreGive(xConsoleMutex);
				break;
			}
		}

		if( (ET_OK == eRet) &&
			(pdTRUE == xTaskCheckForTimeOut(&m_sLastResultPrintTimestamp, &m_sPrintResultsTimeout)))
		{
			this->printResultsStdOut();

			vTaskSetTimeOutState(&m_sLastResultPrintTimestamp);
			m_sPrintResultsTimeout = m_sConfig.sPrintResultsTimeout;
		}

		return eRet;
	}

	ERRORTYPE SensorManager::printResultsStdOut()
	{
		ERRORTYPE eRet =  ET_OK;

		for(uint8_t u8Loop = 0; u8Loop < m_sConfig.u8SensorCount; ++u8Loop)
		{
			uint32_t u32Result = m_sConfig.pSensorList[u8Loop]->getResult();
			xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
			xSerialxPrintf_P( &xSerialPort, PSTR("%s = %i "), m_sConfig.pSensorList[u8Loop]->getDescription(),u32Result);
			xSerialxPrintf_P( &xSerialPort, PSTR("%s\r\n"), m_sConfig.pSensorList[u8Loop]->getSensorUnits());
			xSemaphoreGive(xConsoleMutex);
		}

		return eRet;
	}

} /* namespace SensorManagement */
} /* namespace Application */
