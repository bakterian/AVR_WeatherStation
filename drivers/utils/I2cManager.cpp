/*
 * I2cManager.cpp
 *
 *  Created on: 03.06.2016
 *      Author: Marcin.Karczewski
 */

#include "I2cManager.h"

namespace drivers
{
namespace sensors
{
	I2cManager::I2cManager(const Configuration& rConfig):
			m_sConfig(rConfig),
			m_eInitState(NotInitialized)
	{

	}

	I2cManager::~I2cManager()
	{

	}

	ERRORTYPE I2cManager::initialize()
	{
		ERRORTYPE eRet = ET_OK;

		if(m_eInitState == Initialized)
		{
			eRet = ET_NOK;
		}
		else
		{
			if(m_sConfig.eOperationMode == Master)
			{
				I2C_Master_Initialise( m_sConfig.u16I2cAddress );
			}
			else
			{
				I2C_Slave_Initialise( m_sConfig.u16I2cAddress );
			}
			m_eInitState = Initialized;
		}

		return (eRet);
	}

	ERRORTYPE I2cManager::writeBlocking(uint8_t* pMsg, uint8_t u8MsgSize)
	{
		ERRORTYPE eRet = this->sendI2cData(pMsg,u8MsgSize);

		while((eRet == ET_OK) && I2C_Transceiver_Busy()) {}

		return eRet;
	}

	ERRORTYPE I2cManager::write(uint8_t* pMsg, uint8_t u8MsgSize)
	{
		ERRORTYPE eRet = ET_OK;

		if(I2C_Transceiver_Busy())
		{
			eRet = ET_I2C_BUSY;
		}
		else
		{
			eRet = this->sendI2cData(pMsg,u8MsgSize);
		}

		return eRet;
	}

	ERRORTYPE I2cManager::getI2cBuffer(uint8_t* pStorageBuffer, uint8_t u8NoOfBytesToRead)
	{
		ERRORTYPE eRet = ET_OK;


		if(pStorageBuffer == NULL)
		{
			eRet = ET_NULL_ARG;
		}
		else if(I2C_Transceiver_Busy())
		{
			eRet = ET_I2C_BUSY;
		}
		else
		{
			uint8_t u8Ret = 1;
			if(m_sConfig.eOperationMode == Master)
			{
				u8Ret = I2C_Master_Get_Data_From_Transceiver(pStorageBuffer,u8NoOfBytesToRead);
			}
			else
			{
				u8Ret = I2C_Slave_Get_Data_From_Transceiver(pStorageBuffer, u8NoOfBytesToRead);
			}

			if(u8Ret == 0)
			{
				eRet = ET_NOK;
			}
		}

		return eRet;
	}

	bool I2cManager::wasI2cInitialized()
	{
		return (m_eInitState == Initialized);
	}

	uint8_t I2cManager::getI2cState()
	{
		uint8_t u8I2cState = uint8_t(0xFFU);

		if(!I2C_Transceiver_Busy())
		{
			u8I2cState = I2C_Get_State_InfoNonBlocking();
		}

		return u8I2cState;
	}

	uint16_t I2cManager::getI2cAddress()
	{
		return m_sConfig.u16I2cAddress;
	}

	ERRORTYPE I2cManager::sendI2cData(uint8_t* pMsg, uint8_t u8MsgSize)
	{
		ERRORTYPE eRet = ET_OK;

		if(pMsg == NULL)
		{
			eRet = ET_NULL_ARG;
		}
		else
		{
			if(m_sConfig.eOperationMode == Master)
			{
				I2C_Master_Start_Transceiver_With_Data(pMsg,u8MsgSize);
			}
			else
			{
				I2C_Slave_Start_Transceiver_With_Data(pMsg,u8MsgSize);
			}
		}

		return eRet;
	}

} /* namespace sensors */
} /* namespace drivers */
