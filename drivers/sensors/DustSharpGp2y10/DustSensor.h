/*
 * DustSensor.h
 *
 *  Created on: 15.04.2016
 *      Author: Marcin.Karczewski
 */

#ifndef DRIVERS_SENSORS_DUST_DUSTSENSOR_H_
#define DRIVERS_SENSORS_DUST_DUSTSENSOR_H_

#include "..\ISensor.h"
#include "..\..\..\Configuration\Globals.h"

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
	#define ADC_VREF_MV            		2600Ul
	#define ADC_RESOLUTION		   		1024Ul
	#define MEASUREMENT_COUNT	   		40U

	#define PRE_SAMPLING_TIMEOUT_US 	280.0f
	#define POST_SAMPLING_TIMEOUT_US 	40.0f
	#define SLEEP_TIMEOUT_US 			9680.0f
	#define MEAS_TICK_TIMEOUT			(10 / portTICK_PERIOD_MS)

	/**
	 * \CTOR
	 */
	DustSensor(const ISensor::Configuration& rConfig);

	/**
	 * \brief DTOR
	 */
	virtual ~DustSensor();

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

	/**
	 * brief Called during stand by state - resets the measurement data
	 */
	void doResetOfMeasurementData();

	/**
	 * brief Called during the measuring state - handles cyclic measurement
	 */
	void doMeasuring();

	/**
	 * brief Called during the result calculation state - calculates the dust quantity
	 */
	void doDustCalculation();

	/**
	 * brief Called during the error state - prints error summary message
	 */
	void doErrorStatusPrint();

	/**
	 * brief Called during the error state - prints error summary message
	 */
	void setMeasurement();

private:

	#define						STATE_IDLE			 			0U
	#define						STATE_STAND_BY					1U
	#define						STATE_MEASURING 				2U
	#define						STATE_RESULT_CALCUALATION		3U
	#define						STATE_ERROR						4U

	uint16_t 					m_u16DustQuantity;
	uint16_t 					m_u16AdcResultSum;
	uint8_t						m_u8MeasurementLoop;
	uint8_t						m_u8SensorState;
	uint8_t 					m_u8NewSensorState;
	TickType_t 					m_sMeasTimeout;
	TimeOut_t					m_sMeasTimestamp;
};

} /* namespace sensors */
} /* namespace drivers */

#endif /* DRIVERS_SENSORS_DUST_DUSTSENSOR_H_ */
