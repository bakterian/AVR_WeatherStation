/*
 * TwiManager.cpp
 *
 *  Created on: 12.06.2016
 *      Author: Marcin.Karczewski
 */

#include "TwiManager.h"

namespace drivers
{
namespace sensors
{

// Initialize Class Variables //////////////////////////////////////////////////
uint8_t TwiManager::rxBuffer[BUFFER_LENGTH];
uint8_t TwiManager::rxBufferIndex = 0;
uint8_t TwiManager::rxBufferLength = 0;

uint8_t TwiManager::txAddress = 0;
uint8_t TwiManager::txBuffer[BUFFER_LENGTH];
uint8_t TwiManager::txBufferIndex = 0;
uint8_t TwiManager::txBufferLength = 0;

uint8_t TwiManager::transmitting = 0;
void (*TwiManager::user_onRequest)(void);
void (*TwiManager::user_onReceive)(int);

// Constructors ////////////////////////////////////////////////////////////////

TwiManager::TwiManager(const Configuration& rConfig):
		m_sConfig(rConfig),
		m_eInitState(NotInitialized)
{

}

TwiManager::~TwiManager()
{

}

ERRORTYPE TwiManager::initialize()
{
	ERRORTYPE eRet = ET_OK;

	if(m_eInitState == Initialized)
	{
		eRet = ET_NOK;
	}
	else
	{
		twi_setAddress((uint8_t)m_sConfig.u8I2cAddress);
		twi_attachSlaveTxEvent(onRequestService);
		twi_attachSlaveRxEvent(onReceiveService);
		rxBufferIndex = 0;
		rxBufferLength = 0;

		txBufferIndex = 0;
		txBufferLength = 0;

		twi_init();
		m_eInitState = Initialized;
	}

	return (eRet);
}

bool TwiManager::wasI2cInitialized()
{
	return (m_eInitState == Initialized);
}

void TwiManager::beginTransmission(uint8_t u8TargetAddress)
{
  // indicate that we are transmitting
  transmitting = 1;
  // set address of targeted slave
  txAddress = u8TargetAddress;
  // reset tx buffer iterator vars
  txBufferIndex = 0;
  txBufferLength = 0;
}

//
//	Originally, 'endTransmission' was an f(void) function.
//	It has been modified to take one parameter indicating
//	whether or not a STOP should be performed on the bus.
//	Calling endTransmission(false) allows a sketch to
//	perform a repeated start.
//	WARNING: Nothing in the library keeps track of whether
//	the bus tenure has been properly ended with a STOP. It
//	is very possible to leave the bus in a hung state if
//	no call to endTransmission(true) is made. Some I2C
//	devices will behave oddly if they do not see a STOP.
//
ERRORTYPE TwiManager::endTransmission(uint8_t sendStop)
{
	// transmit buffer (blocking)
	int8_t ret = twi_writeTo(txAddress, txBuffer, txBufferLength, 1, sendStop); 	//TODO: error translation from uint8_t to ERRORTYPE
	// reset tx buffer iterator vars
	txBufferIndex = 0;
	txBufferLength = 0;
	// indicate that we are done transmitting
	transmitting = 0;

	return (ERRORTYPE) ret;
}

ERRORTYPE TwiManager::requestFrom(uint8_t address, uint8_t quantity, uint8_t sendStop)
{
	ERRORTYPE eRet = ET_OK;

	if(quantity > BUFFER_LENGTH)
	{
		eRet = ET_ARG_TOO_BIG;
	}
	else
	{
		// perform blocking read into buffer
		uint8_t u8read = twi_readFrom(address, rxBuffer, quantity, sendStop);

		// set rx buffer iterator vars
		rxBufferIndex = 0;
		rxBufferLength = u8read;

		if(u8read != quantity)
		{
			eRet = ET_NOK;
		}
	}

  return eRet;
}


ERRORTYPE TwiManager::write(uint8_t data)
{
  ERRORTYPE eRet = ET_OK;
  if(transmitting)
  {// in master transmitter mode

    if(txBufferLength >= BUFFER_LENGTH)
    { // don't bother if buffer is full
		//setWriteError(); //TODO: implement function returing current errors
    	eRet = ET_NOK;
    }
    else
    {
		// put byte in tx buffer
		txBuffer[txBufferIndex] = data;
		++txBufferIndex;
		// update amount in buffer
		txBufferLength = txBufferIndex;
    }
  }
  else
  {  // in slave send mode
    twi_transmit(&data, 1);  // reply to master
  }

  return eRet;
}

ERRORTYPE TwiManager::write(const uint8_t *data, size_t quantity)
{
  ERRORTYPE eRet = ET_OK;
  if(transmitting)
  {// in master transmitter mode
    for(size_t i = 0; (i < quantity) && (eRet == ET_OK); ++i)
    {
    	eRet = write(data[i]);
    }
  }
  else
  {// in slave send mode
    twi_transmit(data, quantity);  // reply to master
  }

  return eRet;
}

uint8_t TwiManager::available(void)
{
  return rxBufferLength - rxBufferIndex;
}

uint8_t TwiManager::read(void)
{
  uint8_t value = uint8_t(0xFFU);

  // get each successive byte on each call
  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
    ++rxBufferIndex;
  }

  return value;
}

uint8_t TwiManager::peek(void)
{
  uint8_t value = uint8_t(0xFFU);

  if(rxBufferIndex < rxBufferLength){
    value = rxBuffer[rxBufferIndex];
  }

  return value;
}

uint8_t TwiManager::getI2cAddress()
{
	return m_sConfig.u8I2cAddress;
}


void TwiManager::onReceiveService(uint8_t* inBytes, int numBytes)
{
  // don't bother if user hasn't registered a callback
  if(!user_onReceive){
    return;
  }
  // don't bother if rx buffer is in use by a master requestFrom() op
  // i know this drops data, but it allows for slight stupidity
  // meaning, they may not have read all the master requestFrom() data yet
  if(rxBufferIndex < rxBufferLength){
    return;
  }
  // copy twi rx buffer into local read buffer
  // this enables new reads to happen in parallel
  for(uint8_t i = 0; i < numBytes; ++i){
    rxBuffer[i] = inBytes[i];
  }
  // set rx iterator vars
  rxBufferIndex = 0;
  rxBufferLength = numBytes;
  // alert user program
  user_onReceive(numBytes);
}

void TwiManager::onRequestService(void)
{
  // don't bother if user hasn't registered a callback
  if(!user_onRequest){
    return;
  }
  // reset tx buffer iterator vars
  // !!! this will kill any pending pre-master sendTo() activity
  txBufferIndex = 0;
  txBufferLength = 0;
  // alert user program
  user_onRequest();
}

void TwiManager::onReceive( void (*function)(int) )
{
  user_onReceive = function;
}

void TwiManager::onRequest( void (*function)(void) )
{
  user_onRequest = function;
}


} /* namespace sensors */
} /* namespace drivers */
