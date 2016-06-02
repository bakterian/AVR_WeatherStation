/*
 * TaskStatsProvider.h
 *
 *  Created on: 02.06.2016
 *      Author: Marcin.Karczewski
 */

#ifndef OS_TASKS_UTILS_TASKSTATSPROVIDER_H_
#define OS_TASKS_UTILS_TASKSTATSPROVIDER_H_

#include "../../../Configuration/Globals.h"

namespace OS
{
namespace Tasks
{

class TaskStatsProvider
{
public:

	/**
	 * \brief CTOR
	 */
	TaskStatsProvider(const TickType_t	sPrintTaskStatsTimeout);

	/**
	 * \brief DTOR
	 */
	virtual ~TaskStatsProvider();

   /**
	* \brief printStats function.
	* To be called cyclicly.
	*/
	virtual void printStats();

private:
   /**
	* \brief printTaskStats function.
	* Prints information on the current task.
	*/
	virtual void printTaskStats();

	TimeOut_t			m_sPrintTaskStatsTimestamp;
	TickType_t 			m_sInitialPrintTaskStatsTimeout;
	TickType_t 			m_sPrintTaskStatsTimeout;
};

} /* namespace Tasks */
} /* namespace OS */

#endif /* OS_TASKS_UTILS_TASKSTATSPROVIDER_H_ */
