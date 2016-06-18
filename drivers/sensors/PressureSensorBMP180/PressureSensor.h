/*
 * PressureSensor.h
 *
 *  Created on: 06.06.2016
 *      Author: Marcin.Karczewski
 */

#ifndef DRIVERS_SENSORS_PRESSURESENSORBMP180_PRESSURESENSOR_H_
#define DRIVERS_SENSORS_PRESSURESENSORBMP180_PRESSURESENSOR_H_

#include "..\..\..\Configuration\Globals.h"
#include "..\ISensor.h"
#include "..\..\Utils\wire\TwiManager.h"

namespace drivers
{
namespace sensors
{

class PressureSensor : public ISensor
{
public:

	/***************************************************************/
	/**\name	MISCELLANEOUS DEFINITION      				   	   */
	//#define BMP180_DEBUG 					uint8_t(0x01U)
	#define WRITE_REG_ADDR_CMD_LENGTH		uint8_t(0x02U)
	#define I2C_BUFFER_SIZE_				uint8_t(0x08U)
	#define TICK_TEMP_MEAS_TIMEOUT			(10 / portTICK_PERIOD_MS)
	#define TICK_PRESSURE_MEAS_TIMEOUT		(30 / portTICK_PERIOD_MS)
	#define BMP180_I2C_ADDRESS				uint8_t(0x77U)
	#define BMP_180_TEMP_MEAS_MODE			uint8_t(0x2EU)
	#define BMP_180_PRESSURE_MEAS_CMD		uint8_t(0x34U)
	#define BMP_180_CHIP_ID					uint8_t(0x55U)
	/***************************************************************/


	/***************************************************************/
	/**\name	REGISTER ADDRESS DEFINITION      				   */
	#define	OUT_XLSB_REG_ADDRESS			uint8_t(0xF8U)
	#define	OUT_LSB_REG_ADDRESS				uint8_t(0xF7U)
	#define	OUT_MSB_REG_ADDRESS				uint8_t(0xF6U)
	#define	CTRL_MEAS_REG_ADDRESS			uint8_t(0xF4U)
	#define SOFT_RESET_REG_ADDRESS			uint8_t(0xE0U)
	#define	CHIP_ID_REG_ADDRESS				uint8_t(0xD0U)
	#define	CALIB_DATA_FIRST_ADDRESS		uint8_t(0xAAU)
	#define	CALIB_DATA_LAST_ADDRESS			uint8_t(0xBEU)
	/***************************************************************/

	/****************************************************/
	/**\name	ARRAY SIZE DEFINITIONS      			*/
	#define	BMP180_TEMPERATURE_DATA_BYTES	(2)
	#define	BMP180_PRESSURE_DATA_BYTES		(3)
	#define	BMP180_TEMPERATURE_LSB_DATA		(1)
	#define	BMP180_TEMPERATURE_MSB_DATA		(0)
	#define	BMP180_PRESSURE_MSB_DATA		(0)
	#define	BMP180_PRESSURE_LSB_DATA		(1)
	#define	BMP180_PRESSURE_XLSB_DATA		(2)
	#define	BMP180_CALIB_DATA_SIZE			(22)
	#define	BMP180_CALIB_PARAM_AC1_MSB		(0)
	#define	BMP180_CALIB_PARAM_AC1_LSB		(1)
	#define	BMP180_CALIB_PARAM_AC2_MSB		(2)
	#define	BMP180_CALIB_PARAM_AC2_LSB		(3)
	#define	BMP180_CALIB_PARAM_AC3_MSB		(4)
	#define	BMP180_CALIB_PARAM_AC3_LSB		(5)
	#define	BMP180_CALIB_PARAM_AC4_MSB		(6)
	#define	BMP180_CALIB_PARAM_AC4_LSB		(7)
	#define	BMP180_CALIB_PARAM_AC5_MSB		(8)
	#define	BMP180_CALIB_PARAM_AC5_LSB		(9)
	#define	BMP180_CALIB_PARAM_AC6_MSB		(10)
	#define	BMP180_CALIB_PARAM_AC6_LSB		(11)
	#define	BMP180_CALIB_PARAM_B1_MSB		(12)
	#define	BMP180_CALIB_PARAM_B1_LSB		(13)
	#define	BMP180_CALIB_PARAM_B2_MSB		(14)
	#define	BMP180_CALIB_PARAM_B2_LSB		(15)
	#define	BMP180_CALIB_PARAM_MB_MSB		(16)
	#define	BMP180_CALIB_PARAM_MB_LSB		(17)
	#define	BMP180_CALIB_PARAM_MC_MSB		(18)
	#define	BMP180_CALIB_PARAM_MC_LSB		(19)
	#define	BMP180_CALIB_PARAM_MD_MSB		(20)
	#define	BMP180_CALIB_PARAM_MD_LSB		(21)
	/***************************************************/

	/****************************************************/
	/**\name	SENSOR STATE DEFINITIONS      			*/
	#define ST_IDLE							uint8_t(0x00U)
	#define ST_READING_CALIB_DATA			uint8_t(0x01U)
	#define	ST_TEMP_DATA_ACQUSITION			uint8_t(0x02U)
	#define	ST_PRESSURE_DATA_ACQUSITION		uint8_t(0x03U)
	#define	ST_TEMP_DATA_EVALUATION			uint8_t(0x04U)
	#define	ST_PRESSURE_DATA_EVALUATION		uint8_t(0x05U)
	#define ST_ERROR						uint8_t(0x06U)
	/***************************************************/


	/****************************************************/
	/**\name	CLASS SPECIFIC EXTRA TYPES              */
	/*!
	 * @brief This structure holds all device specific calibration parameters
	 */
	struct Bmp180CalibrationData
	{
		int16_t 	ac1;	/**<calibration ac1 data*/
		int16_t 	ac2;	/**<calibration ac2 data*/
		int16_t 	ac3;	/**<calibration ac3 data*/
		uint16_t	ac4;	/**<calibration ac4 data*/
		uint16_t 	ac5;	/**<calibration ac5 data*/
		uint16_t 	ac6;	/**<calibration ac6 data*/
		int16_t 	b1;		/**<calibration b1 data*/
		int16_t 	b2;		/**<calibration b2 data*/
		int16_t		mb;		/**<calibration mb data*/
		int16_t 	mc;		/**<calibration mc data*/
		int16_t 	md;		/**<calibration md data*/
	};

	/*!
	 * @brief PressureMeasMode is used for pressure measurement mode specification
	 */
	enum PressureMeasMode
	{
		UltraLowPower 	 		= uint8_t(0x00U), // Oversampling off, conv time 4.5 ms, Avg Current 3uA
		Standard		 		= uint8_t(0x01U), // Oversampling with 2 samples, conv time 7 ms, Avg Current 5uA
		HighResolution			= uint8_t(0x02U), // Oversampling with 4 samples, conv time 13.5 ms, Avg Current 7uA
		UltraHighResolution		= uint8_t(0x03U), // Oversampling with 8 samples, conv time 25.5 ms, Avg Current 12uA
	};

	/*!
	 * @brief PressureSesnor Configuration and references
	 */
	struct Configuration
	{
		PressureMeasMode						ePressureMeasMode;	// BMP180 measurement mode
		::drivers::sensors::TwiManager*			pTwiManager;		// Pointer to the I2c Manager
		ISensor::Configuration	 				sBaseConfig;		// ISensor base configuration
	};
	/****************************************************/

	/**
	 * \brief CTOR
	 */
	PressureSensor(const Configuration& rConfig);

	/**
	 * \brief DTOR
	 */
	virtual ~PressureSensor();

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
	 * \brief reading calibration data and verifying the received valued
	 * \return ET_OK - acquired calibration data
	 * \return ET_NOK - there were issues during the calibration data acquisition
	 * \return ET_PENDING - The I2C is not free, therefore we try later
	 */
	virtual ERRORTYPE stateReadingCalibData();

	/**
	 * \brief send i2c temp measurement msg and clock out the uncompensated temperature values
	 * \return ET_OK 		- read uncompensated temperature values without problems
	 * \return ET_NOK		- there were errors during uncompensated temperature values read
	 * \return ET_PENDING 	- The I2C is not free, therefore we try later
	 */
	virtual ERRORTYPE stateTempDataAcquisition();

	/**
	 * \brief  send i2c pressure measurement msg and clock out the uncompensated temperature values
	 * \return ET_OK 		- read uncompensated pressure values without problems
	 * \return ET_NOK 		- there were errors during uncompensated pressure values read
	 * \return ET_PENDING 	- The I2C is not free, therefore we try later
	 */
	virtual ERRORTYPE statePressureDataAcquisition();

	/**
	 * \brief evaluate the stored uncompensated temperature values using the calibration data
	 * \return ET_OK 		- evaluated uncompensated temperature data
	 * \return ET_NOK 		- there were issues during the evaluation
	 */
	virtual ERRORTYPE stateTempDataEvaluation();

	/**
	 * \brief evaluate the stored uncompensated pressure values using the calibration data
	 * \return ET_OK 		- evaluated uncompensated pressure data
	 * \return ET_NOK 		- there were issues during the evaluation
	 */
	virtual ERRORTYPE statePressureDataEvaluation();

	/**
	 * \brief The sensor lands here in case of errors. TODO add error recovery stratergy
	 * \return ET_OK - entered state without issues
	 */
	virtual ERRORTYPE stateError();

private:
	/**
	 * \brief Computes the b5 intermediate variable
	 * \return B5 intermediate variable
	 */
	uint32_t computeB5();

	/*!
	    @brief  Writes an 8 bit value over I2C
	*/
	ERRORTYPE writeCommand(uint8_t reg, uint8_t value);

	/*!
	    @brief  Reads an 8 bit value over I2C
	*/
	ERRORTYPE read8(uint8_t reg, uint8_t *value);

	/*!
	    @brief  Reads a 16 bit value over I2C
	*/
	ERRORTYPE read16(uint8_t reg, uint16_t *value);

	/*!
	    @brief  Reads a 16 bit value over I2C
	*/
	ERRORTYPE read24(uint8_t reg, uint32_t  *value);

	struct SensorStateEntry
	{
		uint8_t 	u8InternalStateId;
		ERRORTYPE 	(PressureSensor::*fptrStateHandler)();
	};

	static const SensorStateEntry 	m_sSensorStateMap[];

	const Configuration	 	m_sConfig;
	uint32_t				m_u32LastTempResult;
	uint32_t				m_u32LastPressureResult;
	uint8_t					m_u8SensorState;
	uint8_t					m_u8NewSensorState;
	Bmp180CalibrationData*	m_psCalibrationData;
	uint8_t*				m_pu8I2cBuffer;
	TimeOut_t				m_sTimeoutStartTimestamp;
	TickType_t 				m_sI2cReadTimeout;
	uint8_t					m_u8CurCalbDataRegAddress;
	uint16_t				m_u16UncompTemperature;
	uint32_t 				m_u32UncompPressure;
};

} /* namespace sensors */
} /* namespace drivers */

#endif /* DRIVERS_SENSORS_PRESSURESENSORBMP180_PRESSURESENSOR_H_ */
