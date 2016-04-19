/*
 * DustSensor.h
 *
 *  Created on: 15.04.2016
 *      Author: Marcin.Karczewski
 */

#ifndef DRIVERS_SENSORS_DUST_DUSTSENSOR_H_
#define DRIVERS_SENSORS_DUST_DUSTSENSOR_H_

#include "..\ISensor.h"

namespace drivers
{
namespace sensors
{

/**
 * \class DustSensor
 * Dust sensor driver.
 */
class DustSensor : public ISensor
{
	public:

	#define MIN_ADC_VOLTAGE_MV     		589U
	#define SENOR_INTERNAL_LED     		PORTB0
	#define SENOR_OUTPUT		   		PINA0
	#define ADC_VREF_MV            		2560Ul
	#define ADC_RESOLUTION		   		1024Ul
	#define MEASUREMENT_COUNT	   		20U

	#define PRE_SAMPLING_TIMEOUT_US 	280.0f
	#define POST_SAMPLING_TIMEOUT_US 	40.0f
	#define SLEEP_TIMEOUT_US 			9680.0f

	/**
	 * \CTOR
	 */
	DustSensor(const ISensor::Configuration& rConfig);

	/**
	 * \brief DTOR
	 */
	virtual ~DustSensor();

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
	 * \return sensor measurement result
	 */
	virtual uint32_t getResult();

protected:
	/**
	 * brief Performs a single measurement
	 */
	uint16_t makeSingleMeas();

	/**
	 * brief Blocks execution and read the adc value.
	 * returns the rescaled voltage value in milivolt
	 */
	uint16_t readAdcValue();

	/**
	 * brief Turns the integrated IR led on.
	 */
	void setLED();

	/**
	 * brief Turns the integrated IR led off.
	 */
	void clearLED();

private:
	uint16_t 					m_u16DustQuantity;
	uint16_t 					m_u16AdcResultSum;
};

} /* namespace sensors */
} /* namespace drivers */

#endif /* DRIVERS_SENSORS_DUST_DUSTSENSOR_H_ */
