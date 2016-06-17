/*
 * PressureSensor.cpp
 *
 *  Created on: 06.06.2016
 *      Author: Marcin.Karczewski
 */

#include "PressureSensor.h"

namespace drivers
{
namespace sensors
{
	const PressureSensor::SensorStateEntry PressureSensor::m_sSensorStateMap[] =
	{
		{ST_IDLE, 							&PressureSensor::stateIdle						},
		{ST_READING_CALIB_DATA, 			&PressureSensor::stateReadingCalibData			},
		{ST_TEMP_DATA_ACQUSITION, 			&PressureSensor::stateTempDataAcquisition		},
		{ST_PRESSURE_DATA_ACQUSITION, 		&PressureSensor::statePressureDataAcquisition	},
		{ST_TEMP_DATA_EVALUATION, 			&PressureSensor::stateTempDataEvaluation		},
		{ST_PRESSURE_DATA_EVALUATION, 		&PressureSensor::statePressureDataEvaluation	},
		{ST_ERROR, 							&PressureSensor::stateError						}
	};

	PressureSensor::PressureSensor(const Configuration& rConfig):
			ISensor(rConfig.sBaseConfig),
			m_sConfig(rConfig),
			m_u32LastTempResult(0xFFFFFFFFU),
			m_u32LastPressureResult(0xFFFFFFFFU),
			m_u8SensorState(ST_IDLE),
			m_u8NewSensorState(ST_IDLE),
			m_sI2cReadTimeout(TICK_TEMP_MEAS_TIMEOUT),
			m_u8CurCalbDataRegAddress(CALIB_DATA_FIRST_ADDRESS),
			m_u16UncompTemperature(0U),
			m_u32UncompPressure(0U)
		{
			m_pu8I2cBuffer = (uint8_t*)pvPortMalloc(I2C_BUFFER_SIZE_*sizeof(uint8_t));
			m_psCalibrationData = (Bmp180CalibrationData*)pvPortMalloc(sizeof(Bmp180CalibrationData));
		}

	PressureSensor::~PressureSensor()
	{
		free(m_psCalibrationData);
		free(m_pu8I2cBuffer);
	}

	ERRORTYPE PressureSensor::initialize()
	{
		ERRORTYPE eRet = ET_OK;

		if(false == m_sConfig.pTwiManager->wasI2cInitialized())
		{
			eRet =	m_sConfig.pTwiManager->initialize();
		}

		if(eRet == ET_OK)
		{
			m_u8NewSensorState = ST_READING_CALIB_DATA;
		}

		return eRet;
	}

	ERRORTYPE PressureSensor::run()
	{
		ERRORTYPE eRet = ((*this).*PressureSensor::m_sSensorStateMap[m_u8NewSensorState].fptrStateHandler)();
		return (eRet);
	}

	uint8_t PressureSensor::getSensorState() const
	{
		return m_u8NewSensorState;
	}

	uint32_t PressureSensor::getResult(MeasDataType eMeasDataType)
	{
		uint32_t u32Ret = 0xFFFFFFFFU;

		if(eMeasDataType == Temperature)
		{
			u32Ret = m_u32LastTempResult;
		}
		else if(eMeasDataType == Pressure)
		{
			u32Ret = m_u32LastPressureResult;
		}

		return u32Ret;
	}

	ERRORTYPE PressureSensor::stateIdle()
	{
		ERRORTYPE eRet =  ET_OK;

		// is this the first state entry?
		if(m_u8NewSensorState != m_u8SensorState)
		{
			#if defined(BMP180_DEBUG)
			xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
			xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| Entering Idle state, system time:%u ms.\r\n"), xTaskGetAbsolutTimeMs());
			xSemaphoreGive(xConsoleMutex);
			#endif

			m_u8SensorState = m_u8NewSensorState;
		}

		return eRet;
	}

	ERRORTYPE PressureSensor::stateReadingCalibData()
	{
		ERRORTYPE eRet =  ET_OK;

		// is this the first state entry?
		if(m_u8NewSensorState != m_u8SensorState)
		{
			#if defined(BMP180_DEBUG)
			xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
			xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| Entering reading calibration data state, system time:%u ms.\r\n"), xTaskGetAbsolutTimeMs());
			xSemaphoreGive(xConsoleMutex);
			#endif

			vTaskSuspendAll();
			uint8_t u8Bmp180ChipId = 0;
			eRet = read8(CHIP_ID_REG_ADDRESS, &u8Bmp180ChipId);

			if((eRet != ET_OK) || (u8Bmp180ChipId != BMP_180_CHIP_ID))
			{
				m_u8NewSensorState = ST_ERROR;
			}
			else
			{
				m_u8SensorState = m_u8NewSensorState;
				m_u8CurCalbDataRegAddress = CALIB_DATA_FIRST_ADDRESS;
			}
			xTaskResumeAll();
		}
		else if( (m_u8CurCalbDataRegAddress >= CALIB_DATA_FIRST_ADDRESS) && (m_u8CurCalbDataRegAddress <= CALIB_DATA_LAST_ADDRESS) )
		{
			vTaskSuspendAll();
			uint16_t u16CalibrationData;
			eRet = read16(m_u8CurCalbDataRegAddress, &u16CalibrationData);

			if((eRet != ET_OK))
			{
				m_u8NewSensorState = ST_ERROR;
			}
			else
			{
				uint8_t u8Offset = (m_u8CurCalbDataRegAddress - CALIB_DATA_FIRST_ADDRESS) / uint8_t(2U);

				if((u8Offset >= uint8_t(3U)) && (u8Offset <= uint8_t(5U)))
				{
					uint16_t* pu16CalibDataBuffer = (uint16_t*) m_psCalibrationData;
					pu16CalibDataBuffer[u8Offset] = u16CalibrationData;
				}
				else
				{
					int16_t* pi16CalibDataBuffer = (int16_t*) m_psCalibrationData;
					pi16CalibDataBuffer[u8Offset] = (int16_t) u16CalibrationData;
				}

				if(m_u8CurCalbDataRegAddress == CALIB_DATA_LAST_ADDRESS)
				{//CalibrationData read is complete go to next stage

					#if defined(BMP180_DEBUG)
					xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
					xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| m_psCalibrationData->ac1 = :%i.\r\n"), m_psCalibrationData->ac1);
					xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| m_psCalibrationData->ac2 = :%i.\r\n"), m_psCalibrationData->ac2);
					xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| m_psCalibrationData->ac3 = :%i.\r\n"), m_psCalibrationData->ac3);
					xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| m_psCalibrationData->ac4 = :%u.\r\n"), m_psCalibrationData->ac4);
					xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| m_psCalibrationData->ac5 = :%u.\r\n"), m_psCalibrationData->ac5);
					xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| m_psCalibrationData->ac6 = :%u.\r\n"), m_psCalibrationData->ac6);
					xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| m_psCalibrationData->b1 = :%i.\r\n"), m_psCalibrationData->b1);
					xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| m_psCalibrationData->b2 = :%i.\r\n"), m_psCalibrationData->b2);
					xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| m_psCalibrationData->mb = :%i.\r\n"), m_psCalibrationData->mb);
					xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| m_psCalibrationData->mc = :%i.\r\n"), m_psCalibrationData->mc);
					xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| m_psCalibrationData->md = :%i.\r\n"), m_psCalibrationData->md);
					xSemaphoreGive(xConsoleMutex);
					#endif

					m_u8NewSensorState = ST_TEMP_DATA_ACQUSITION;
				}
				else
				{
					m_u8CurCalbDataRegAddress = m_u8CurCalbDataRegAddress +2;
				}
			}
			xTaskResumeAll();
		}
		return eRet;
	}

	ERRORTYPE PressureSensor::stateTempDataAcquisition()
	{
		ERRORTYPE eRet =  ET_OK;

		// is this the first state entry?
		if(m_u8NewSensorState != m_u8SensorState)
		{
			#if defined(BMP180_DEBUG)
			xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
			xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| Entering temperature data acquisition, system time:%u ms.\r\n"), xTaskGetAbsolutTimeMs());
			xSemaphoreGive(xConsoleMutex);
			#endif

			vTaskSuspendAll();
		    eRet = writeCommand(CTRL_MEAS_REG_ADDRESS, BMP_180_TEMP_MEAS_MODE);
			if(eRet == ET_OK)
			{
				m_u8SensorState = m_u8NewSensorState;
				vTaskSetTimeOutState(&m_sTimeoutStartTimestamp);
				m_sI2cReadTimeout = TICK_TEMP_MEAS_TIMEOUT;
			}
			else
			{
				m_u8NewSensorState = ST_ERROR;
			}
			xTaskResumeAll();
		}
		else if( pdTRUE == xTaskCheckForTimeOut(&m_sTimeoutStartTimestamp, &m_sI2cReadTimeout) )
		{//timeout elapsed the result should be ready
			vTaskSuspendAll();
			eRet = read16(OUT_MSB_REG_ADDRESS, &m_u16UncompTemperature);
			if((eRet != ET_OK) )
			{
				m_u8NewSensorState = ST_ERROR;
			}
			else
			{
				m_u8NewSensorState = ST_PRESSURE_DATA_ACQUSITION;
			}
			xTaskResumeAll();
		}

		return eRet;
	}

	ERRORTYPE PressureSensor::statePressureDataAcquisition()
	{
		ERRORTYPE eRet =  ET_OK;

		// is this the first state entry?
		if(m_u8NewSensorState != m_u8SensorState)
		{
			#if defined(BMP180_DEBUG)
			xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
			xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| Entering state pressure data acquisition, system time:%u ms.\r\n"), xTaskGetAbsolutTimeMs());
			xSemaphoreGive(xConsoleMutex);
			#endif

			vTaskSuspendAll();
			//Sending start pressure measurement command
			eRet = writeCommand(CTRL_MEAS_REG_ADDRESS, BMP_180_PRESSURE_MEAS_CMD + (((uint8_t)m_sConfig.ePressureMeasMode) << 6));
			if(eRet == ET_OK)
			{
				m_u8SensorState = m_u8NewSensorState;
				vTaskSetTimeOutState(&m_sTimeoutStartTimestamp);
				m_sI2cReadTimeout = TICK_PRESSURE_MEAS_TIMEOUT;
			}
			else
			{
				m_u8NewSensorState = ST_ERROR;
			}
			xTaskResumeAll();
		}
		else if( pdTRUE == xTaskCheckForTimeOut(&m_sTimeoutStartTimestamp, &m_sI2cReadTimeout) )
		{//timeout elapsed the result should be ready
			vTaskSuspendAll();
			eRet = read24(OUT_MSB_REG_ADDRESS, &m_u32UncompPressure);
			if(eRet == ET_OK)
			{
				m_u32UncompPressure >>= (8U - ((uint32_t)m_sConfig.ePressureMeasMode));
				m_u8NewSensorState = ST_TEMP_DATA_EVALUATION;
			}
			else
			{
				m_u8NewSensorState = ST_ERROR;
			}
			xTaskResumeAll();
		}

		return eRet;
	}

	ERRORTYPE PressureSensor::stateTempDataEvaluation()
	{
		ERRORTYPE eRet =  ET_OK;

		// is this the first state entry?
		if(m_u8NewSensorState != m_u8SensorState)
		{
			#if defined(BMP180_DEBUG)
			xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
			xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| Entering temperature data evaluation state, system time:%u ms.\r\n"), xTaskGetAbsolutTimeMs());
			xSemaphoreGive(xConsoleMutex);
			#endif

			m_u8SensorState = m_u8NewSensorState;
		}
		else
		{
			int32_t B5 = computeB5();
			m_u32LastTempResult = uint32_t(100U) *((B5+8) >> 4);
			m_u8NewSensorState = ST_PRESSURE_DATA_EVALUATION;
		}

		return eRet;
	}

	ERRORTYPE PressureSensor::statePressureDataEvaluation()
	{
		ERRORTYPE eRet =  ET_OK;

		// is this the first state entry?
		if(m_u8NewSensorState != m_u8SensorState)
		{
			#if defined(BMP180_DEBUG)
			xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
			xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| Entering pressure data evaluation state, system time:%u ms.\r\n"), xTaskGetAbsolutTimeMs());
			xSemaphoreGive(xConsoleMutex);
			#endif

			m_u8SensorState = m_u8NewSensorState;
		}
		else
		{
			int32_t p;
			/* Temperature compensation */
			int32_t b5 = (int32_t)computeB5();

			/* Pressure compensation */
			int32_t b6 = b5 - 4000;
			int32_t x1 = (m_psCalibrationData->b2 * ((b6 * b6) >> 12)) >> 11;
			int32_t x2 = (m_psCalibrationData->ac2 * b6) >> 11;
			int32_t x3 = x1 + x2;
			int32_t b3 = (((((int32_t) m_psCalibrationData->ac1) * 4 + x3) << ((uint32_t)m_sConfig.ePressureMeasMode)) + 2) >> 2;
			x1 = (m_psCalibrationData->ac3 * b6) >> 13;
			x2 = (m_psCalibrationData->b1 * ((b6 * b6) >> 12)) >> 16;
			x3 = ((x1 + x2) + 2) >> 2;
			uint32_t b4 = (m_psCalibrationData->ac4 * (uint32_t) (x3 + 32768)) >> 15;
			uint32_t b7 = ((uint32_t) (m_u32UncompPressure - b3) * (50000 >> ((uint32_t)m_sConfig.ePressureMeasMode)));

			if (b7 < 0x80000000)
			{
				p = (b7 << 1) / b4;
			}
			else
			{
				p = (b7 / b4) << 1;
			}

			x1 = (p >> 8) * (p >> 8);
			x1 = (x1 * 3038) >> 16;
			x2 = (-7357 * p) >> 16;

			m_u32LastPressureResult = (uint32_t) (p + ((x1 + x2 + 3791) >> 4));
			m_u8NewSensorState = ST_TEMP_DATA_ACQUSITION;
		}

		return eRet;
	}

	ERRORTYPE PressureSensor::stateError()
	{
		ERRORTYPE eRet =  ET_OK;

		// is this the first state entry?
		if(m_u8NewSensorState != m_u8SensorState)
		{
			xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
			xSerialxPrintf_P( &xSerialPort, PSTR("|Pressure Sensor| Entering Error state , system time:%u ms.\r\n"), xTaskGetAbsolutTimeMs());
			xSemaphoreGive(xConsoleMutex);
			m_u8SensorState = m_u8NewSensorState;
		}

		return eRet;
	}

	uint32_t PressureSensor::computeB5()
	{
		int32_t X1 = (((int32_t)m_u16UncompTemperature) - ((int32_t)(m_psCalibrationData->ac6))) * ((int32_t)m_psCalibrationData->ac5) >> 15U;
		int32_t X2 = ((int32_t)m_psCalibrationData->mc << 11) / (X1+(int32_t)m_psCalibrationData->md);
		return (X1 + X2);
	}

	ERRORTYPE PressureSensor::writeCommand(uint8_t reg, uint8_t value)
	{
		m_sConfig.pTwiManager->beginTransmission(BMP180_I2C_ADDRESS);
		m_sConfig.pTwiManager->write(reg);
		m_sConfig.pTwiManager->write(value);
		ERRORTYPE eRet = m_sConfig.pTwiManager->endTransmission(uint8_t(1U));

		return eRet;
	}

	ERRORTYPE PressureSensor::read8(uint8_t reg, uint8_t *value)
	{
		ERRORTYPE eRet = ET_OK;
		m_sConfig.pTwiManager->beginTransmission(BMP180_I2C_ADDRESS);
		m_sConfig.pTwiManager->write(reg);
		eRet = m_sConfig.pTwiManager->endTransmission(uint8_t(1U));

		if(eRet == ET_OK)
		{
			eRet = m_sConfig.pTwiManager->requestFrom(BMP180_I2C_ADDRESS, uint8_t(1U),  uint8_t(1U));
		}

		if(eRet == ET_OK)
		{
			*value = m_sConfig.pTwiManager->read();
			eRet = m_sConfig.pTwiManager->endTransmission(uint8_t(1U));
		}

		return eRet;
	}

	ERRORTYPE PressureSensor::read16(uint8_t reg, uint16_t *value)
	{
		ERRORTYPE eRet = ET_OK;
		m_sConfig.pTwiManager->beginTransmission(BMP180_I2C_ADDRESS);
		m_sConfig.pTwiManager->write(reg);
		eRet = m_sConfig.pTwiManager->endTransmission(uint8_t(1U));

		if(eRet == ET_OK)
		{
			eRet = m_sConfig.pTwiManager->requestFrom(BMP180_I2C_ADDRESS, uint8_t(2U),  uint8_t(1U));
		}

		if(eRet == ET_OK)
		{
			*value = (((uint16_t)(m_sConfig.pTwiManager->read()) << 8U) | m_sConfig.pTwiManager->read());
			eRet = m_sConfig.pTwiManager->endTransmission(uint8_t(1U));
		}

		return eRet;
	}

	ERRORTYPE PressureSensor::read24(uint8_t reg, uint32_t *value)
	{
		ERRORTYPE eRet = ET_OK;
		m_sConfig.pTwiManager->beginTransmission(BMP180_I2C_ADDRESS);
		m_sConfig.pTwiManager->write(reg);
		eRet = m_sConfig.pTwiManager->endTransmission(uint8_t(1U));

		if(eRet == ET_OK)
		{
			eRet = m_sConfig.pTwiManager->requestFrom(BMP180_I2C_ADDRESS, uint8_t(3U),  uint8_t(1U));
		}

		if(eRet == ET_OK)
		{
			*value = ((((uint32_t)(m_sConfig.pTwiManager->read())) << 16U) | (((uint32_t)(m_sConfig.pTwiManager->read())) << 8U) | m_sConfig.pTwiManager->read());
			eRet = m_sConfig.pTwiManager->endTransmission(uint8_t(1U));
		}

		return eRet;
	}

} /* namespace sensors */
} /* namespace drivers */
