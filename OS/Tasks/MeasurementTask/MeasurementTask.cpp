/*
 * MeasurementTask.cpp
 *
 *  Created on: 16.04.2016
 *      Author: Marcin.Karczewski
 */

#include "MeasurementTask.h"

namespace OS
{
namespace Tasks
{

	MeasurementTask::MeasurementTask(const TaskConfiguration& rConfiguration):
	TaskClass(rConfiguration.csName, rConfiguration.ePriority, rConfiguration.u16StackDepth),
	m_sConfig(rConfiguration),
	TaskStatsProvider(rConfiguration.sPrintTaskStatsTimeout)
	{

	}

	MeasurementTask::~MeasurementTask()
	{

	}

	void MeasurementTask::task()
	{
		ERRORTYPE eRet = m_sConfig.sSensorManager.initSensors();

        if (ET_OK != eRet)
        {
        	xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
        	xSerialxPrintf_P( &xSerialPort, PSTR("\r\nProblems during sensor initialization, system time:%u ms.\r\n"), xTaskGetAbsolutTimeMs());
    		xSemaphoreGive(xConsoleMutex);
    		return;
        }

		while(1)
		{
			this->printStats();

			eRet = m_sConfig.sSensorManager.run();
			if(ET_OK != eRet)
			{
		        xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
		        xSerialxPrintf_P( &xSerialPort, PSTR("\r\nMeasurement Errors, system time:%u ms.\r\n"), xTaskGetAbsolutTimeMs());
		        xSemaphoreGive(xConsoleMutex);
			}
		};
	}

} /* namespace Tasks */
} /* namespace OS */
