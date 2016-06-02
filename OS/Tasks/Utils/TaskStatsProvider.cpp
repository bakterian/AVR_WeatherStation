/*
 * TaskStatsProvider.cpp
 *
 *  Created on: 02.06.2016
 *      Author: Marcin.Karczewski
 */

#include "TaskStatsProvider.h"

namespace OS
{
namespace Tasks
{
	TaskStatsProvider::TaskStatsProvider(const TickType_t	sPrintTaskStatsTimeout):
		m_sPrintTaskStatsTimestamp(),
		m_sPrintTaskStatsTimeout(sPrintTaskStatsTimeout),
		m_sInitialPrintTaskStatsTimeout(sPrintTaskStatsTimeout)
	{

	}

	TaskStatsProvider::~TaskStatsProvider()
	{

	}

	void TaskStatsProvider::printStats()
	{
		if(pdTRUE == xTaskCheckForTimeOut(&m_sPrintTaskStatsTimestamp, &m_sPrintTaskStatsTimeout))
		{
			this->printTaskStats();

			vTaskSetTimeOutState(&m_sPrintTaskStatsTimestamp);
			m_sPrintTaskStatsTimeout = m_sInitialPrintTaskStatsTimeout;
		}
	}

	void TaskStatsProvider::printTaskStats()
	{
	   xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
	   xSerialxPrintf_P( &xSerialPort, PSTR("| %s | Stack HighWater @ %u, system time: %u ms. \r\n"), pcTaskGetTaskName(NULL), uxTaskGetStackHighWaterMark(NULL),xTaskGetAbsolutTimeMs());
	   xSemaphoreGive(xConsoleMutex);
	}

} /* namespace Tasks */
} /* namespace OS */
