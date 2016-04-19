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
	m_sConfig(rConfiguration)
	{

	}

	MeasurementTask::~MeasurementTask()
	{

	}

	void MeasurementTask::task()
	{
		ERRORTYPE eRet = m_sConfig.sSensorManager.InitSensors();
		TickType_t xLastWakeTime;
		xLastWakeTime = xTaskGetTickCount();

        xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
        if (ET_OK == eRet)
        {
    		avrSerialxPrint_P( &xSerialPort, PSTR("\r\nSensor Initialization was successful.\r\n"));
        }
        else
        {
    		avrSerialxPrint_P( &xSerialPort, PSTR("\r\nProblems during sensor initialization.\r\n"));
        }
        xSemaphoreGive(xConsoleMutex);

		while(1)
		{
			eRet = m_sConfig.sSensorManager.Run(xSerialPort, xConsoleMutex);
			if(ET_OK != eRet)
			{
		        xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
		        if (ET_PENDING == eRet)
		        {
		    		avrSerialxPrint_P( &xSerialPort, PSTR("\r\nMeasurment is taking too long.\r\n"));
		        }
		        else
		        {
		    		avrSerialxPrint_P( &xSerialPort, PSTR("\r\nMeasurement Errors.\r\n"));
		        }
		        xSemaphoreGive(xConsoleMutex);
			}

			vTaskDelayUntil( &xLastWakeTime, ( 1000 / portTICK_PERIOD_MS ) );
		};
	}

} /* namespace Tasks */
} /* namespace OS */
