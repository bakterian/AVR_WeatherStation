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
		ISensor**		 pSensorList;			// list containing ISensors
		uint8_t	 		 u8SensorCount;			// number of elements in sensor list
		TickType_t 		 sPrintResultsTimeout;	// tick count which needs to pass from last result print
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
	 * Brief initialize sensors
	 * return ET_OK if completed successfully
	 */
	ERRORTYPE initSensors();

	/*
	 * Brief  run measurements
	 * return ET_OK if completed successfully
	 */
	ERRORTYPE run();

	/*
	 * Brief  Update the serial trace with the latest results
	 * return ET_OK if completed successfully
	 */
	ERRORTYPE printResultsStdOut();

private:
	SensorConfiguration 	m_sConfig;
	TimeOut_t				m_sLastResultPrintTimestamp;
	TickType_t 				m_sPrintResultsTimeout;
};

} /* namespace SensorManagement */
} /* namespace Application */

#endif /* APPLICATION_SENSORMANAGEMENT_SENSORMANAGER_H_ */
