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

enum MeasDataType
{
	Dust,
	Voltage,
	Humidity,
	Temperature,
	Light,
	Pressure,
	RainDrops
};

/**
 * \Interface ISensor
 * Provides a common sensor interface.
 */
class ISensor
{
public:

	struct MeasurementDataInfo
	{
		PGM_P 					psMeasDataDescription;			  /*  measurement data description 	*/
		PGM_P	 				psUnits;					   	  /*  measurement data units	  	*/
		MeasDataType			eMeasDataType;			      	  /*  measurement data type 	    */
	};

	struct Configuration
	{
		MeasurementDataInfo*	pMeasurmentDataList;			  /*   measurement data list  		*/
		uint8_t					u8MeasurmentDataCount;			  /*   measurement data count 		*/

	};

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
	virtual uint32_t getResult(MeasDataType eMeasDataType);

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
	 * \brief provides the measurement data structure
	 */
	MeasurementDataInfo getMeasDataInfo(uint8_t u8MeasDataNo) const;

	/**
	 * \brief provides the measurement data count
	 */
	uint8_t getMeasDataCount() const;

private:
	Configuration m_ISensorConfig;
};

} /* namespace sensors */
} /* namespace drivers */
#endif /* DRIVERS_SENSORS_ISENSOR_H_ */
