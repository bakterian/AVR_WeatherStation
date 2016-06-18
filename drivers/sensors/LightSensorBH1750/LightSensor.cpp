/*
 * LightSensor.cpp
 *
 *  Created on: 04.06.2016
 *      Author: Marcin.Karczewski
 */

#include "LightSensor.h"

namespace drivers
{
namespace sensors
{
	const LightSensor::SensorStateEntry LightSensor::m_sSensorStateMap[] =
	{
		{STATE_IDLE_, 				&LightSensor::stateIdle					},
		{STATE_LOAD_CONFIG_, 		&LightSensor::stateLoadConfig			},
		{STATE_CLOCKING_OUT_DATA_, 	&LightSensor::stateClockingOutResult	},
		{STATE_RESULT_ACQUISITION_, &LightSensor::stateResultAcquisition	},
		{STATE_ERROR_, 				&LightSensor::stateError				}
	};

	LightSensor::LightSensor(const Configuration& rConfig):
		ISensor(rConfig.sBaseConfig),
		m_sConfig(rConfig),
		m_u32LastResult(0),
		m_u8SensorState(STATE_IDLE_),
		m_u8NewSensorState(STATE_IDLE_),
		m_sMeasuringTimeout(TICK_MEAS_MODE_SET_TIMEOUT)
	{
		m_pu8I2cBuffer = (uint8_t*)pvPortMalloc(RESULT_READ_CMD_LENGTH*sizeof(uint8_t));
	}

	LightSensor::~LightSensor()
	{
		free(m_pu8I2cBuffer);
	}

	ERRORTYPE LightSensor::initialize()
	{
		ERRORTYPE eRet = ET_OK;

		if(false == m_sConfig.pTwiManager->wasI2cInitialized())
		{
			eRet =	m_sConfig.pTwiManager->initialize();
		}

		if(eRet == ET_OK)
		{
			m_u8NewSensorState = STATE_LOAD_CONFIG_;
		}
		return eRet;

		return ET_OK;
	}

	ERRORTYPE LightSensor::run()
	{
		ERRORTYPE eRet = ((*this).*LightSensor::m_sSensorStateMap[m_u8NewSensorState].fptrStateHandler)();

		return (eRet);
	}

	uint8_t LightSensor::getSensorState() const
	{
		return m_u8NewSensorState;
	}

	uint32_t LightSensor::getResult(MeasDataType eMeasDataType)
	{
		return m_u32LastResult;
	}

	ERRORTYPE LightSensor::stateIdle()
	{
		// is this the first state entry?
		if(m_u8NewSensorState != m_u8SensorState)
		{
			#if defined(BH1750_DEBUG)
			xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
			xSerialxPrintf_P( &xSerialPort, PSTR("|Light Sensor| Entering idle state, system time:%u ms.\r\n"), xTaskGetAbsolutTimeMs());
			xSemaphoreGive(xConsoleMutex);
			#endif
			m_u8SensorState = m_u8NewSensorState;
		}

		return (ET_OK);
	}

	ERRORTYPE LightSensor::stateLoadConfig()
	{
		ERRORTYPE eRet =  ET_OK;

		// is this the first state entry?
		if(m_u8NewSensorState != m_u8SensorState)
		{
			#if defined(BH1750_DEBUG)
			xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
			xSerialxPrintf_P( &xSerialPort, PSTR("|Light Sensor| Entering load config state, system time:%u ms.\r\n"), xTaskGetAbsolutTimeMs());
			xSemaphoreGive(xConsoleMutex);
			#endif

			if(false == m_sConfig.pTwiManager->wasI2cInitialized())
			{
				eRet =	m_sConfig.pTwiManager->initialize();
			}

			if(eRet != ET_OK)
			{
				m_u8NewSensorState = STATE_ERROR_;
			}
			else
			{
				m_u8SensorState = m_u8NewSensorState;
			}
		}
		else
		{

			vTaskSuspendAll();
			m_sConfig.pTwiManager->beginTransmission((uint8_t) m_sConfig.eI2cAdress);
			m_sConfig.pTwiManager->write((uint8_t) m_sConfig.eOperationMode);
			eRet= m_sConfig.pTwiManager->endTransmission(uint8_t(1U));
			xTaskResumeAll();

			if(eRet == ET_OK)
			{  //sensor starts measurement and needs at least 180ms
				vTaskSetTimeOutState(&m_sMeasStartTimestamp);
				m_sMeasuringTimeout = TICK_MEAS_MODE_SET_TIMEOUT;

				m_u8NewSensorState = STATE_CLOCKING_OUT_DATA_;
			}
			else
			{
				m_u8NewSensorState = STATE_ERROR_;
			}
		}

		return eRet;
	}

	ERRORTYPE LightSensor::stateClockingOutResult()
	{
		ERRORTYPE eRet =  ET_OK;

		// is this the first state entry?
		if(m_u8NewSensorState != m_u8SensorState)
		{
			#if defined(BH1750_DEBUG)
			xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
			xSerialxPrintf_P( &xSerialPort, PSTR("|Light Sensor| Entering clocking out result state, system time:%u ms.\r\n"), xTaskGetAbsolutTimeMs());
			xSemaphoreGive(xConsoleMutex);
			#endif

			m_u8SensorState = m_u8NewSensorState;
		}
		else
		{
			if( pdTRUE == xTaskCheckForTimeOut(&m_sMeasStartTimestamp, &m_sMeasuringTimeout) )
			{//timeout elapsed the result should be ready

				vTaskSuspendAll();
				eRet = m_sConfig.pTwiManager->requestFrom((uint8_t) m_sConfig.eI2cAdress, uint8_t(2U),  uint8_t(1U));
				if(eRet == ET_OK)
				{
					m_u32LastResult = (((uint16_t)(m_sConfig.pTwiManager->read()) << 8U) | m_sConfig.pTwiManager->read());
				}
				xTaskResumeAll();

				if(eRet == ET_OK)
				{
					m_u8NewSensorState = STATE_RESULT_ACQUISITION_;
				}
				else
				{
					m_u8NewSensorState = STATE_ERROR_;
				}
			}
		}

		return eRet;
	}

	ERRORTYPE LightSensor::stateResultAcquisition()
	{
		ERRORTYPE eRet =  ET_OK;

		// is this the first state entry?
		if(m_u8NewSensorState != m_u8SensorState)
		{
			#if defined(BH1750_DEBUG)
			xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
			xSerialxPrintf_P( &xSerialPort, PSTR("|Light Sensor| Entering result acquisition state, system time:%u ms.\r\n"), xTaskGetAbsolutTimeMs());
			xSemaphoreGive(xConsoleMutex);
			#endif

			m_u8SensorState = m_u8NewSensorState;
		}
		else
		{
			//calculating the lux value (works only for two modes)
			//m_u32LastResult = (((((uint32_t)m_pu8I2cBuffer[1]) << 8U) | ((uint32_t)m_pu8I2cBuffer[2])) * uint32_t(10U)) /uint32_t(12U);
			m_u32LastResult = (m_u32LastResult * uint32_t(10U)) / uint32_t(12U);

			if((uint8_t)m_sConfig.eOperationMode < uint8_t(0x20))
			{//sensor is running in continuous mode, therefore we only need gather results
				vTaskSetTimeOutState(&m_sMeasStartTimestamp);
				m_sMeasuringTimeout = TICK_MEASUREMENT_TIMEOUT;
				m_u8NewSensorState = STATE_CLOCKING_OUT_DATA_;
			}
			else
			{//the sensor is making single measurements and is in power down mode
				m_u8NewSensorState = STATE_LOAD_CONFIG_;
			}
		}

		return eRet;
	}

	ERRORTYPE LightSensor::stateError()
	{
		ERRORTYPE eRet =  ET_OK;

		// is this the first state entry?
		if(m_u8NewSensorState != m_u8SensorState)
		{
			xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
			xSerialxPrintf_P( &xSerialPort, PSTR("|Light Sensor| Entering error state, system time:%u ms.\r\n"), xTaskGetAbsolutTimeMs());
			xSemaphoreGive(xConsoleMutex);

			m_u8SensorState = m_u8NewSensorState;
		}

		return eRet;
	}

} /* namespace sensors */
} /* namespace drivers */
