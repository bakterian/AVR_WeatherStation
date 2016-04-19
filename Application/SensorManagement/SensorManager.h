/*
 * SensorManager.h
 *
 *  Created on: 17.04.2016
 *      Author: Marcin.Karczewski
 */

#ifndef APPLICATION_SENSORMANAGEMENT_SENSORMANAGER_H_
#define APPLICATION_SENSORMANAGEMENT_SENSORMANAGER_H_

#include "../../Configuration/Globals.h"
#include "../../drivers/sensors/ISensor.h"

namespace Application
{
namespace SensorManagement
{
using namespace ::drivers::sensors;
/*
 * @Brief Class - Responsible for sensor measurement coordination
 */
class SensorManager {
public:
	struct SensorConfiguration
	{
		ISensor* pSensorList;
		uint8_t	 u8SensorCount;
	};
	/*
	 * Brief CTOR
	 */
	SensorManager(const SensorConfiguration& rConfig);

	/*
	 * Brief DTOR
	 */
	virtual ~SensorManager();

	/*
	 * Brief initialze sensors
	 * return ET_OK if completed successfully
	 */
	ERRORTYPE InitSensors();

	/*
	 * Brief run measurements
	 * return ET_OK if completed successfully
	 */
	ERRORTYPE Run(xComPortHandle& rConsole, QueueHandle_t& rConsoleMutex);

private:
	SensorConfiguration m_sConfig;
};

} /* namespace SensorManagement */
} /* namespace Application */

#endif /* APPLICATION_SENSORMANAGEMENT_SENSORMANAGER_H_ */
