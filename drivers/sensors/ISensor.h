/*
 * ISensor.h
 *
 *  Created on: 15.04.2016
 *      Author: Marcin.Karczewski
 */

#ifndef DRIVERS_SENSORS_ISENSOR_H_
#define DRIVERS_SENSORS_ISENSOR_H_

#include "..\..\Configuration\Globals.h"
#include "commonTypes.h"
#include "stdint.h"

namespace drivers
{
namespace sensors
{

enum SensorType
{
	eDustSensor,
	eVoltageSensor,
	eHumiditySensor,
	eTemperatureSensor,
	eLightSensor,
	ePressureSensor,
	eRainSensor
};

/**
 * \Interface ISensor
 * Provides a common sensor interface.
 */
class ISensor {
public:

	/**
	 * \brief cyclicly triggers measurement start
	 * \return ET_OK - measurement finished without problems
	 * \return ET_OK_NOT - measurement finished with problems
	 * \return ET_PENDING - measurement is ongoing
	 */
	virtual ERRORTYPE run();

	/**
	 * \brief delivers the measurement result.
	 */
	virtual uint32_t getResult() { return 0xFFFFFFFF; }

	struct Configuration
	{
		PGM_P 		psSensorDescription;				 /* sensor description */
		PGM_P	 	psSensorUnits;						/*  sensor units	  */
		SensorType	eSensorType;			  		   /*   sensor type	     */
	};

	/**
	 * \brief CTOR
	 */
	ISensor(const Configuration& rConfig);

	/**
	 * \brief DTOR
	 */
	virtual ~ISensor();

	/**
	 * \brief triggers sensor initialization
	 * \return ET_OK - sensor was initialized
	 */
	virtual ERRORTYPE initialize();

	/**
	 * \brief provides the sensor state
	 * \return sensor state
	 */
	virtual uint8_t getSensorState() const;

	/**
	 * \brief provides the sensor description
	 * \return pointer to cstring stored in flash with the sensor description
	 */
	PGM_P getDescription() const;

	/**
	 * \brief provides the sensor units
	 * \return pointer to cstring stored in flash with the sensor units
	 */
	PGM_P getSensorUnits() const;

	/**
	 * \brief provides the sensor type
	 * \return sensor type
	 */
	SensorType getSensorType() const;

private:
	Configuration m_ISensorConfig;
};

} /* namespace sensors */
} /* namespace drivers */
#endif /* DRIVERS_SENSORS_ISENSOR_H_ */
