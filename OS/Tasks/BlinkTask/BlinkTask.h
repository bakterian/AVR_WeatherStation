/*
 * BlinkTask.h
 *
 *  Created on: 16.04.2016
 *      Author: Marcin.Karczewski
 */

#ifndef OS_TASKS_BLINKTASK_BLINKTASK_H_
#define OS_TASKS_BLINKTASK_BLINKTASK_H_

#include "../../../Configuration/Globals.h"

namespace OS
{
namespace Tasks
{

class BlinkTask : TaskClass
{
	public:
	/**
	* \brief Task Configuration.
	*/
	struct TaskConfiguration
	{
		char const* csName;
		TaskPriority ePriority;
		unsigned int u16StackDepth;
		unsigned int u16BlinkIntervalMs;
		//xComPortHandle hConsoleOut;
		//QueueHandle_t hConsoleMutex;
	};

	/**
	 * \brief CTOR
	 */
	BlinkTask(const TaskConfiguration& rConfiguration);

	/**
	 * \brief DTOR
	 */
	virtual ~BlinkTask();

	/**
	* \brief Task function.
	* The member function task needs to call by the RTOS, this one should include a infinite loop
	*/
	virtual void task();

	private:
	TaskConfiguration m_sConfig;
};

} /* namespace Tasks */
} /* namespace OS */

#endif /* OS_TASKS_BLINKTASK_BLINKTASK_H_ */
