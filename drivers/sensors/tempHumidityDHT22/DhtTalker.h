/*
 * DhtTalker.h
 *
 *  Created on: 06.05.2016
 *      Author: Marcin.Karczewski
 */

#ifndef DRIVERS_SENSORS_TEMPHUMIDITYDHT22_DHTTALKER_H_
#define DRIVERS_SENSORS_TEMPHUMIDITYDHT22_DHTTALKER_H_

#include "..\..\..\Configuration\Globals.h"

namespace drivers
{
namespace sensors
{

class DhtTalker
{
public:
	#define DATA_BUFFER_SIZE 				5U
	#define VOLTAGE_PULSE_COUNT 			80U
	#define MAX_PULSE_COUNT					0xFFFFFFFFU
	#define	PULSE_COUNT_TIMEDOUT			0U

	#define TALKER_PIN						PB1
	#define TALKER_PORT						PORTB
	#define TALKER_DDR						DDRB

	#define TIMEOUT_AFTER_HANDSHAKE_US		40U
	#define TIMEOUT_BEFORE_READ_US			10U
	#define TIMEOUT_READ_START_US			80U

	#define STATE_IDLE						0U
	#define STATE_PRE_INIT					1U
	#define STATE_RECEIVING_DATA			3U
	#define STATE_EVALUATING_DATA			4U
	#define STATE_ERRORS					5U

	enum VoltagePulseType
	{
		eLow,
		eHigh
	};

	struct Configuration
	{
		TickType_t 		sLastMeasTimeout;				//tick count which needs to pass from last result read
		TickType_t 		sInitialLowStateTimeout;		//tick count which needs to pass in order to reach proper low pin state
	};

	/**
	 * \CTOR
	 */
	DhtTalker(const Configuration& rConfig);

	/**
	 * \DTOR
	 */
	virtual ~DhtTalker();

	/**
	 * brief Retrieves the data stored in the buffer
	 * \return pointer to a data buffer
	 * \return null-pointer if data is not available
	 */
	uint8_t* getReceivedData();

	/**
	 * brief Retrieves the talker state
	 * \return state machine value
	 */
	uint8_t getTalkerState();

	/**
	 * \return ET_OK - no problems were encountered
	 * brief Initialize the talker
	 * \return ET_OK_NOT - the were errors
	 */
	ERRORTYPE init();

	/**
	 * brief Runtime function which coordinates the internal state machine
	 * \return ET_OK - no problems were encountered
	 * \return ET_OK_NOT - the were errors
	 */
	ERRORTYPE run();

private:

	/**
	 * brief Initial talkerState
	 * \return ET_OK - no problems were encountered
	 */
	ERRORTYPE stateIdle();

	/**
	 * brief Sets the 1 wire pin high and allows the pull up voltage to build up
	 * \return ET_OK - no problems were encountered
	 * \return ET_OK_NOT - the were errors
	 */
	ERRORTYPE statePreInit();

	/**
	 * brief Receives data from DHT22 - this is a critical section and task can not be interrupted
	 * \return ET_OK - no problems were encountered
	 * \return ET_OK_NOT - the were errors
	 */
	ERRORTYPE stateReceivingData();

	/**
	 * brief Evaluating the received data - determines single bit values
	 * \return ET_OK - no problems were encountered
	 * \return ET_OK_NOT - the were errors
	 */
	ERRORTYPE stateEvaluatingData();

	/**
	 * brief The talker lands here in case of errors.
	 * \return ET_OK - no problems were encountered
	 * \return ET_OK_NOT - the were errors
	 */
	ERRORTYPE stateError();

	/**
	 * brief Provides the pulse duration
	 * \return voltage pulse time duration in us
	 * \return 0 in case of an timeout
	 */
	uint32_t getPulseDuration(VoltagePulseType ePulseType);

	struct TalkerStateEntry
	{
		uint8_t 	u8InternalStateId;
		ERRORTYPE 	(DhtTalker::*fptrStateHandler)();
	};

	static const TalkerStateEntry 	m_sTalkerStateMap[];

	uint32_t 				m_au32PulseContainer[VOLTAGE_PULSE_COUNT];
	uint8_t 				m_au8DataBuffer[DATA_BUFFER_SIZE];
	uint8_t					m_u8TalkerState;
	uint8_t					m_u8NewTalkerState;
	TimeOut_t				m_sMeasFinishedTimestamp;
	TimeOut_t				m_sHandshakeStartTimestamp;
	Configuration			m_sConfig;
};

} /* namespace sensors */
} /* namespace drivers */

#endif /* DRIVERS_SENSORS_TEMPHUMIDITYDHT22_DHTTALKER_H_ */
