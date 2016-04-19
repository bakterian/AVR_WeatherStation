/*
 * ISensor.h
 *
 *  Created on: 15.04.2016
 *      Author: Marcin.Karczewski
 */

#ifndef DRIVERS_SENSORS_ISENSOR_H_
#define DRIVERS_SENSORS_ISENSOR_H_

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
	eTemperatureSensor,
	ePressureSensor,
	eRainSensor
};

/**
 * \Interface ISensor
 * Provides a common sensor interface.
 */
class ISensor {
public:

	struct Configuration
	{
		const uint8_t* csSensorDescription;
		const uint8_t* csSensorUnits;
		SensorType	eSensorType;
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
	 * \brief cyclicly triggers measurement start
	 * \return ET_OK - sensor was initialized
	 */
	virtual ERRORTYPE initialize();

	/**
	 * \brief cyclicly triggers measurement start
	 * \return ET_OK - measurement finished without problems
	 * \return ET_OK_NOT - measurement finished with problems
	 * \return ET_PENDING - measurement is ongoing
	 */
	virtual ERRORTYPE Run();

	/**
	 * \brief delivers the measurement result.
	 */
	virtual uint32_t getResult() { return 0xFFFFFFFF; }

	/**
	 * \brief provides the sensor description
	 * \return pointer to cstring with the sensor description
	 */
	const uint8_t* getDescription() const;

	/**
	 * \brief provides the sensor units
	 * \return pointer to cstring with the sensor units
	 */
	const uint8_t* getSensorUnits() const;

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
