/*
 * TwiManager.h
 *
 *  Created on: 12.06.2016
 *      Author: Marcin.Karczewski
 */

#ifndef DRIVERS_UTILS_WIRE_TWIMANAGER_H_
#define DRIVERS_UTILS_WIRE_TWIMANAGER_H_

#include "..\..\..\Configuration\Globals.h"
#include "twi.h"

namespace drivers
{
namespace sensors
{

#define BUFFER_LENGTH 32

/**
 * \Classs TwiManager
 * Provides a interface to communicate over i2c using the hardware TWI interface
 */
class TwiManager
{
public:
	enum I2cInitState
	{
		NotInitialized  			= uint8_t(0),
		Initialized 				= uint8_t(1)
	};

	struct Configuration
	{
		const uint8_t		u8I2cAddress;		 	/* i2c address 			  */
	};

	/**
	 * \brief CTOR
	 */
	TwiManager(const Configuration& rConfig);

	/**
	 * \brief DTOR
	 */
	virtual ~TwiManager();

	/**
	 * \brief triggers i2c initialization
	 * \return ET_OK - intialization was succesfull
	 */
	ERRORTYPE initialize();

	/**
	 * \brief Returns information if the i2c line was initialized.
	 * \return false	- i2c was not initialized
	 * \return true		- i2c was initialized
	 */
	bool wasI2cInitialized();

	void beginTransmission(uint8_t u8TargetAddress);

	ERRORTYPE endTransmission(uint8_t sendStop);

	ERRORTYPE requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop);

	// must be called in:
	// slave tx event callback
	// or after beginTransmission(address)
	ERRORTYPE write(uint8_t data);

	// must be called in:
	// slave tx event callback
	// or after beginTransmission(address)
	ERRORTYPE write(const uint8_t *data, size_t quantity);

	// must be called in:
	// slave rx event callback
	// or after requestFrom(address, numBytes)
	uint8_t available(void);

	// must be called in:
	// slave rx event callback
	// or after requestFrom(address, numBytes)
	uint8_t read(void);

	// must be called in:
	// slave rx event callback
	// or after requestFrom(address, numBytes)
	uint8_t peek(void);

	/**
	 * \brief Provides the I2C address passsed in the configuration.
	 * \return uint8_t  - i2c address
	 */
	uint8_t getI2cAddress();

	/**
	 * \brief sets function called on slave write
	 * \param function - callback function
	 */
	void onReceive( void (*function)(int) );

	/**
	 * \brief sets function called on slave read
	 * \param function - callback function
	 */
	void onRequest( void (*function)(void) );

	/*
	 * nice To have:
	 * read1Bytes
	 * read2Bytes
	*/

private:
	Configuration m_sConfig;
	I2cInitState m_eInitState;

    static uint8_t rxBuffer[];
    static uint8_t rxBufferIndex;
    static uint8_t rxBufferLength;

    static uint8_t txAddress;
    static uint8_t txBuffer[];
    static uint8_t txBufferIndex;
    static uint8_t txBufferLength;

    static uint8_t transmitting;

	static void (*user_onRequest)(void);
	static void (*user_onReceive)(int);

	// behind the scenes function that is called when data is requested
	static void onRequestService(void);

	// behind the scenes function that is called when data is received
	static void onReceiveService(uint8_t*, int);
};

} /* namespace sensors */
} /* namespace drivers */

#endif /* DRIVERS_UTILS_WIRE_TWIMANAGER_H_ */
