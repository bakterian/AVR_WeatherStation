/*
 * TempHumiditySensor.h
 *
 *  Created on: 18.04.2016
 *      Author: Marcin.Karczewski
 */
#ifndef DRIVERS_SENSORS_TEMPHUMIDITYDHT22_TEMPHUMIDITYSENSOR_H_
#define DRIVERS_SENSORS_TEMPHUMIDITYDHT22_TEMPHUMIDITYSENSOR_H_

#include "..\ISensor.h"
#include "DhtTalker.h"
namespace drivers
{
namespace sensors
{

/**
 * \class TempHumiditySensor
 *  Driver for the DHT22 Temperature and humidity sensor
 */
class TempHumiditySensor : public ISensor
{
public:

	struct DhtMeasurmentData
	{
		uint16_t u16Humidity;
		uint16_t u16Temperature;
		uint8_t	 u8Crc;
	};

	struct DhtConfiguartion
	{
		DhtTalker*				pDhtTalker;
		DhtMeasurmentData 		sDhtMeasurementData;
		ISensor::Configuration 	sBaseConfig;
	};

	/**
	 * \CTOR
	 */
	TempHumiditySensor(const DhtConfiguartion& rConfig);

	/**
	 * \DTOR
	 */
	virtual ~TempHumiditySensor();

	/**
	 * \brief triggers sensor initialization
	 * \return ET_OK - sensor was initialized
	 */
	virtual ERRORTYPE initialize();

	/**
	 * \brief cyclicly triggers measurement start
	 * \return ET_OK - measurement finished without problems
	 * \return ET_OK_NOT - measurement finished with problems
	 * \return ET_PENDING - measurement is ongoing
	 */
	virtual ERRORTYPE run();

	/**
	 * \brief delivers the measurement result.
	 * \return sensor measurement result
	 */
	virtual uint32_t getResult(MeasDataType eMeasDataType);

private:
	DhtConfiguartion 			m_sDhtConfig;
};

} /* namespace sensors */
} /* namespace drivers */

#endif /* DRIVERS_SENSORS_TEMPHUMIDITYDHT22_TEMPHUMIDITYSENSOR_H_ */
