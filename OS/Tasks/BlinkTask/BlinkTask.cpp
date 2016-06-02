/*
 * BlinkTask.cpp
 *
 *  Created on: 16.04.2016
 *      Author: Marcin.Karczewski
 */

#include "BlinkTask.h"

namespace OS
{
namespace Tasks
{

BlinkTask::BlinkTask(const TaskConfiguration& rConfiguration):
TaskClass(rConfiguration.csName, rConfiguration.ePriority, rConfiguration.u16StackDepth),
m_sConfig(rConfiguration),
TaskStatsProvider(rConfiguration.sPrintTaskStatsTimeout)
{

}

BlinkTask::~BlinkTask()
{

}

void BlinkTask::task()
{
	TickType_t xLastWakeTime;
	/* The xLastWakeTime variable needs to be initialised with the current tick
	count.  Note that this is the only time we access this variable.  From this
	point on xLastWakeTime is managed automatically by the vTaskDelayUntil()
	API function. */
	xLastWakeTime = xTaskGetTickCount();

	DDRA |= _BV(DDA1) | _BV(DDA2);

	while(1)
	{
		this->printStats();
		PORTA |=  _BV(PORTA1);       // main (yellow PA1) LED on.
		PORTA &= ~_BV(PORTA2);       // main (blue IO_A2) LED off.

		vTaskDelayUntil( &xLastWakeTime, ( m_sConfig.u16BlinkIntervalMs / portTICK_PERIOD_MS ) );

		PORTA &= ~_BV(PORTA1);       // main (yellow PA1) LED off.
    	PORTA |=  _BV(PORTA2);       // main (blue IO_A2) LED on.

		vTaskDelayUntil( &xLastWakeTime, ( m_sConfig.u16BlinkIntervalMs / portTICK_PERIOD_MS ) );

		/*
       xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
       xSerialxPrintf_P( &xSerialPort, PSTR("| Blink Task | Stack HighWater @ %u, system time:%u ms.\r\n"), uxTaskGetStackHighWaterMark(NULL), xTaskGetAbsolutTimeMs());
       xSemaphoreGive(xConsoleMutex);
       */
	}
}

} /* namespace Tasks */
} /* namespace OS */
