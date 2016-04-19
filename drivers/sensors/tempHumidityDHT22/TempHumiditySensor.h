/*
 * TempHumiditySensor.h
 *
 *  Created on: 18.04.2016
 *      Author: Marcin.Karczewski
 */
#ifndef DRIVERS_SENSORS_TEMPHUMIDITYDHT22_TEMPHUMIDITYSENSOR_H_
#define DRIVERS_SENSORS_TEMPHUMIDITYDHT22_TEMPHUMIDITYSENSOR_H_

namespace drivers
{
namespace sensors
{

class TempHumiditySensor
{
	public:
	/**
	 * \CTOR
	 */
	TempHumiditySensor();

	/**
	 * \DTOR
	 */
	virtual ~TempHumiditySensor();
};

} /* namespace sensors */
} /* namespace drivers */

#endif /* DRIVERS_SENSORS_TEMPHUMIDITYDHT22_TEMPHUMIDITYSENSOR_H_ */
