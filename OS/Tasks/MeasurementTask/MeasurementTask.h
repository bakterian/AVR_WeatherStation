/*
 * MeasurementTask.h
 *
 *  Created on: 16.04.2016
 *      Author: Marcin.Karczewski
 */

#ifndef OS_TASKS_MEASUREMENTTASK_MEASUREMENTTASK_H_
#define OS_TASKS_MEASUREMENTTASK_MEASUREMENTTASK_H_

#include "../../../Configuration/Globals.h"
#include "../../../Application/SensorManagement/SensorManager.h"

namespace OS
{
namespace Tasks
{

/**
 * \Class MeasurementTask
 * Runs the measurement task
 */
class MeasurementTask : TaskClass
{
	public:
   /**
    * \brief Task Configuration.
    */
	struct TaskConfiguration
	{
		char const* 									csName;
		TaskPriority 									ePriority;
		unsigned int 									u16StackDepth;
		Application::SensorManagement::SensorManager 	sSensorManager;
	};

	/**
	 * \brief CTOR
	 */
	MeasurementTask(const TaskConfiguration& rConfiguration);

	/**
	 * \brief DTOR
	 */
	virtual ~MeasurementTask();

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

#endif /* OS_TASKS_MEASUREMENTTASK_MEASUREMENTTASK_H_ */
