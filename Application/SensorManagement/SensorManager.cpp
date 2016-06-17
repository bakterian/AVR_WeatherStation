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
				xSerialPrint_P(PSTR("/SENSOR MGR/ "));
				xSerialPrint_P(m_sConfig.pSensorList[u8Loop]->getMeasDataInfo(0).psMeasDataDescription);
				xSerialxPrintf_P( &xSerialPort, PSTR(", in state %u, encountered problems.\r\n"),m_sConfig.pSensorList[u8Loop]->getSensorState());
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

		//iterate through each sensor
		for(uint8_t u8SensorNo = 0; u8SensorNo < m_sConfig.u8SensorCount; ++u8SensorNo)
		{
			//iterate through each measurment data type which is provided by the sensor
			for(uint8_t u8MeasDataNo = 0; u8MeasDataNo < m_sConfig.pSensorList[u8SensorNo]->getMeasDataCount(); ++u8MeasDataNo)
			{
				ISensor::MeasurementDataInfo eMeasDataInfo = m_sConfig.pSensorList[u8SensorNo]->getMeasDataInfo(u8MeasDataNo);
				uint32_t u32Result = m_sConfig.pSensorList[u8SensorNo]->getResult(eMeasDataInfo.eMeasDataType);

				xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
				xSerialPrint_P(PSTR("|Sensor Mgr| "));
				xSerialPrint_P(eMeasDataInfo.psMeasDataDescription);
				xSerialxPrintf_P( &xSerialPort, PSTR(" = %lu "),u32Result);
				xSerialPrint_P(eMeasDataInfo.psUnits);
				xSerialPrint_P(PSTR("\r\n"));
				xSemaphoreGive(xConsoleMutex);
			}
		}

		return eRet;
	}

} /* namespace SensorManagement */
} /* namespace Application */
