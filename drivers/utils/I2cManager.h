/*
 * I2cManager.h
 *
 *  Created on: 03.06.2016
 *      Author: Marcin.Karczewski
 */

#ifndef DRIVERS_UTILS_I2CMANAGER_H_
#define DRIVERS_UTILS_I2CMANAGER_H_

#include "..\..\Configuration\Globals.h"
#include "i2cMultiMaster.h"

namespace drivers
{
namespace sensors
{


/**
 * \Classs I2cManager
 * Provides a interface to communicate over i2c
 */
class I2cManager
{
public:
	/* ------------------- TWI Status Codes -------------------*/
	#define I2C_ERROR		  					uint8_t(0x00U),
	#define I2C_START_TRANSMITTED				uint8_t(0x08U),
	#define I2C_REPEATED_ST_TRANSMITTED			uint8_t(0x10U),
	#define I2C_NO_INFORMATION					uint8_t(0xF8U),
	#define I2C_INFO_NOT_AVAILABLE				uint8_t(0xFFU),
	/* -------------------------------------------------------*/

	enum I2cInitialisationState
	{
		NotInitialized  			= uint8_t(0),
		Initialized 				= uint8_t(1)
	};

	enum I2cOperationMode
	{
		Slave  						= uint8_t(0),
		Master 						= uint8_t(1)
	};

	struct Configuration
	{
		const uint16_t		u16I2cAddress;		 	/* i2c address 			  */
		I2cOperationMode	eOperationMode;			/*  i2c operation mode	  */
	};

	/**
	 * \brief CTOR
	 */
	I2cManager(const Configuration& rConfig);

	/**
	 * \brief DTOR
	 */
	virtual ~I2cManager();

	/**
	 * \brief triggers i2c initialization
	 * \return ET_OK - intialization was succesfull
	 */
	ERRORTYPE initialize();

	/**
	 * \brief Writes the given data on i2c.
	 * 		  The data mast contain a valid i2c address.
	 * 		  This function blocks execution till the i2c line is free.
	 * \param pMsg 				- pointer to the i2c message data
	 * \param u8MsgSize 		- the message size
	 * \return ET_ARG_TOO_BIG	- the message size is too big
	 * \return ET_NOK 			- write could not be completed
	 * \return ET_OK 			- write was successful
	 */
	ERRORTYPE writeBlocking(uint8_t* pMsg, uint8_t u8MsgSize);

	/**
	 * \brief Writes the given data on i2c.
	 * 		  The data mast contain a valid i2c address.
	 * 		  In case of a busy i2c line the function returns.
	 * \param pMsg 				- pointer to the i2c message data
	 * \param u8MsgSize 		- the message size
	 * \return ET_I2C_BUSY		- the i2c line is busy
	 * \return ET_ARG_TOO_BIG	- the message size is too big
	 * \return ET_NULL_ARG		- the pointer is not holding a memory address
	 * \return ET_NOK 			- write could not be completed
	 * \return ET_OK 			- write was successful
	 */
	ERRORTYPE write(uint8_t* pMsg, uint8_t u8MsgSize);

	/**
	 * \brief Copies i2c buffer data to the provided buffer storage.
	 * 		  In case of a busy i2c line the function returns.
	 * \param  pStorageBuffer 	- pointer to a storage location
	 * \return ET_I2C_BUSY		- the i2c line is busy
	 * \return ET_NULL_ARG		- the pointer is not holding a memory address
	 * \return ET_NOK 			- there were errors
	 * \return ET_OK 			- filled the buffer
	 */
	ERRORTYPE getI2cBuffer(uint8_t* pStorageBuffer, uint8_t u8NoOfBytesToRead);

	/**
	 * \brief Returns information if the i2c line was initialized.
	 * \return false	- i2c was not initialized
	 * \return true		- i2c was initialized
	 */
	bool wasI2cInitialized();

	/**
	 * \brief Provides the I2C state.
	 * \return I2cState  - current i2c state
	 */
	uint8_t getI2cState();

	/**
	 * \brief Provides the I2C address passsed in the configuration.
	 * \return uint8_t  - i2c address
	 */
	uint16_t getI2cAddress();
private:
	/**
	 * \brief Sends the given data on i2c.
	 * 		  The data mast contain a valid i2c address.
	 * \param pMsg 				- pointer to the i2c message data
	 * \param u8MsgSize 		- the message size
	 * \return ET_ARG_TOO_BIG	- the message size is too big
	 * \return ET_NULL_ARG		- the pointer is not holding a memory address
	 * \return ET_NOK 			- write could not be completed
	 * \return ET_OK 			- write was successful
	 */
	ERRORTYPE sendI2cData(uint8_t* pMsg, uint8_t u8MsgSize);

	Configuration m_sConfig;
	I2cInitialisationState m_eInitState;
};

} /* namespace sensors */
} /* namespace drivers */

#endif /* DRIVERS_UTILS_I2CMANAGER_H_ */
