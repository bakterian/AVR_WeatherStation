/*
 * LightSensor.h
 *
 *  Created on: 04.06.2016
 *      Author: Marcin.Karczewski
 */

#ifndef DRIVERS_SENSORS_LIGHTSENSORBH1750_LIGHTSENSOR_H_
#define DRIVERS_SENSORS_LIGHTSENSORBH1750_LIGHTSENSOR_H_

#include "..\..\..\Configuration\Globals.h"
#include "..\ISensor.h"
#include "..\..\Utils\wire\TwiManager.h"

namespace drivers
{
namespace sensors
{

class LightSensor : public ISensor
{
public:
	/***************************************************************/
	/**\name	MISCELLANEOUS DEFINITION      				   	   */
	//#define BH1750_DEBUG 					uint8_t(0x01U) 			// to activate extra debug traces uncomment
	#define BH1750_POWER_DOWN				uint8_t(0x00U) 			// No active state
	#define BH1750_POWER_ON 				uint8_t(0x01U) 			// Wating for measurment command
	#define BH1750_RESET 					uint8_t(0x07U) 			// Reset data register value - not accepted in POWER_DOWN mode
	#define MEAS_MODE_CMD_LENGTH			uint8_t(0x02)
	#define RESULT_READ_CMD_LENGTH			uint8_t(0x03)
	#define TICK_MEASUREMENT_TIMEOUT		(120 / portTICK_PERIOD_MS)
	#define TICK_MEAS_MODE_SET_TIMEOUT		(180 / portTICK_PERIOD_MS)
	/***************************************************************/

	/****************************************************/
	/**\name	SENSOR STATE DEFINITIONS      			*/
	#define STATE_IDLE_						uint8_t(0x00U)
	#define	STATE_LOAD_CONFIG_				uint8_t(0x01U)
	#define STATE_CLOCKING_OUT_DATA_		uint8_t(0x02U)
	#define STATE_RESULT_ACQUISITION_		uint8_t(0x03U)
	#define STATE_ERROR_					uint8_t(0x04U)
	/****************************************************/

	/****************************************************/
	/**\name	CLASS SPECIFIC EXTRA TYPES              */
	/*!
	 * @brief BH1750OperationMode is used for measurement mode specification
	 */
	enum BH1750OperationMode
	{
		ContinousHighResMode 	 = uint8_t(0x10U), // Start measurement at 1lx resolution. Measurement time is approx 120ms.
		ContinousHighResMode2	 = uint8_t(0x11U), // Start measurement at 0.5lx resolution. Measurement time is approx 120ms.
		ContinousLowResMode		 = uint8_t(0x13U), // Start measurement at 4lx resolution. Measurement time is approx 16ms.
		OneTimeHighResMode		 = uint8_t(0x20U), // Start measurement at 1lx resolution. Measurement time is approx 120ms.
		OneTimeHighResMode2		 = uint8_t(0x21U), // Start measurement at 0.5lx resolution. Measurement time is approx 120ms.
		OneTimeLowResMode		 = uint8_t(0x23U) // Start measurement at 1lx resolution. Measurement time is approx 120ms.
	};

	/*!
	 * @brief BH1750I2cAddress is used for I2C Address specification, is dependent on the address pin voltage level
	 */
	enum BH1750I2cAddress
	{
		AddressV1 	 = uint8_t(0x23U), // The ADDR is pulled low
		AddressV2	 = uint8_t(0x58U)  // The ADDR is pulled high
	};

	/*!
	 * @brief PressureSesnor Configuration and references
	 */
	struct Configuration
	{
		BH1750OperationMode						eOperationMode;		// BH1750 measurement mode
		BH1750I2cAddress						eI2cAdress;			// BH1750 I2C Address variant
		::drivers::sensors::TwiManager*			pTwiManager;		// Pointer to the I2c Manager
		ISensor::Configuration	 				sBaseConfig;		// ISensor base configuration
	};
	/****************************************************/

	/**
	 * \brief CTOR
	 */
	LightSensor(const Configuration& rConfig);

	/**
	 * \brief DTOR
	 */
	virtual ~LightSensor();

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
	 * \brief provides the sensor state
	 * \return sensor state
	 */
	virtual uint8_t getSensorState() const;

	/**
	 * \brief delivers the measurement result.
	 * \return sensor measurement result
	 */
	virtual uint32_t getResult(MeasDataType eMeasDataType);

protected:

	/**
	 * \brief state idle
	 * \return ET_OK - entered state without issues
	 */
	virtual ERRORTYPE stateIdle();

	/**
	 * \brief load initial configuration
	 * \return ET_OK - acquired results
	 * \return ET_NOK - there were issues during the result acquisition
	 * \return ET_PENDING - The I2C is not free, therefore we try later
	 */
	virtual ERRORTYPE stateLoadConfig();

	/**
	 * \brief send i2c message and clock out the response
	 * \return ET_OK 		- measurement finished without problems
	 * \return ET_NOK		- measurement finished with problems
	 * \return ET_PENDING 	- The I2C is not free, therefore we try later
	 */
	virtual ERRORTYPE stateClockingOutResult();

	/**
	 * \brief acquire the measurment results
	 * \return ET_OK 		- acquired results
	 * \return ET_NOK 		- there were issues during the result acquisition
	 * \return ET_PENDING 	- The I2C is not free, therefore we try later
	 */
	virtual ERRORTYPE stateResultAcquisition();

	/**
	 * \brief The talker lands here in case of errors. TODO add error recovery stratergy
	 * \return ET_OK - entered state without issues
	 */
	virtual ERRORTYPE stateError();

	struct SensorStateEntry
	{
		uint8_t 	u8InternalStateId;
		ERRORTYPE 	(LightSensor::*fptrStateHandler)();
	};

	static const SensorStateEntry 	m_sSensorStateMap[];

private:

	/**
	 * \brief The talker lands here in case of errors. TODO add error recovery stratergy
	 * \return ET_OK - entered state without issues
	 */
	//ERRORTYPE readMeasResult();

	//ERRORTYPE setOperationMode();



	const Configuration	 	m_sConfig;
	uint32_t				m_u32LastResult;
	uint8_t					m_u8SensorState;
	uint8_t					m_u8NewSensorState;
	uint8_t*				m_pu8I2cBuffer;
	TimeOut_t				m_sMeasStartTimestamp;
	TickType_t 				m_sMeasuringTimeout;			//tick count which needs to pass from measurement start
};

} /* namespace sensors */
} /* namespace drivers */

#endif /* DRIVERS_SENSORS_LIGHTSENSORBH1750_LIGHTSENSOR_H_ */
