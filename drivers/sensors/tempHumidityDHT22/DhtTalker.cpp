/*
 * DhtTalker.cpp
 *
 *  Created on: 06.05.2016
 *      Author: Marcin.Karczewski
 */

#include "DhtTalker.h"
#include <string.h>

namespace drivers
{
namespace sensors
{

	const DhtTalker::TalkerStateEntry DhtTalker::m_sTalkerStateMap[] =
	{
		{STATE_IDLE, 				&DhtTalker::stateIdle				},
		{STATE_PRE_INIT, 			&DhtTalker::statePreInit			},
		{STATE_RECEIVING_DATA, 		&DhtTalker::stateReceivingData		},
		{STATE_EVALUATING_DATA, 	&DhtTalker::stateEvaluatingData		},
		{STATE_ERRORS, 				&DhtTalker::stateError				}
	};

	DhtTalker::DhtTalker() :
		m_u8TalkerState(STATE_IDLE),
		m_u8NewTalkerState(STATE_IDLE),
		m_sLastMeasTimeout(TICK_TIMEOUT_LAST_MEAS)
	{
		m_pu8PulseContainer = (uint8_t*)pvPortMalloc(VOLTAGE_PULSE_COUNT*sizeof(uint8_t));
		memset(&m_au8DataBuffer,0x00U,DATA_BUFFER_SIZE);
	}

	DhtTalker::~DhtTalker()
	{
		vPortFree(m_pu8PulseContainer);
	}

	uint8_t* DhtTalker::getReceivedData()
	{
		return &m_au8DataBuffer[0];
	}

	uint8_t DhtTalker::getTalkerState()
	{
		return m_u8TalkerState;
	}

	ERRORTYPE DhtTalker::init()
	{
		TALKER_DDR 	&= ~(1 << TALKER_BIT);	// setting talker pin as input
		TALKER_PORT	|=  (1 << TALKER_BIT);	// using the internal pull-up

		m_u8NewTalkerState = STATE_PRE_INIT;

		return (ET_OK);
	}

	ERRORTYPE DhtTalker::run()
	{
		ERRORTYPE eRet = ((*this).*DhtTalker::m_sTalkerStateMap[m_u8NewTalkerState].fptrStateHandler)();

		return (eRet);
	}

	ERRORTYPE DhtTalker::stateIdle()
	{
		// is this the first state entry?
		if(m_u8NewTalkerState != m_u8TalkerState)
		{
			m_u8TalkerState = m_u8NewTalkerState;
		}

		return (ET_OK);
	}

	ERRORTYPE DhtTalker::statePreInit()
	{
		// is this the first state entry?
		if(m_u8NewTalkerState != m_u8TalkerState)
		{
			vTaskSetTimeOutState(&m_sMeasFinishedTimestamp);
			m_sLastMeasTimeout = TICK_TIMEOUT_LAST_MEAS;
			m_u8TalkerState = m_u8NewTalkerState;
		}
		else
		{
			if( pdTRUE == xTaskCheckForTimeOut(&m_sMeasFinishedTimestamp, &m_sLastMeasTimeout) )
			{//timeout elapsed new serial read can be started
				m_u8NewTalkerState = STATE_RECEIVING_DATA;
			}
		}
		return (ET_OK);
	}

	ERRORTYPE DhtTalker::stateReceivingData()
	{
		ERRORTYPE eRet = ET_OK;

		// is this the first state entry?
		if(m_u8NewTalkerState != m_u8TalkerState)
		{
			m_u8TalkerState = m_u8NewTalkerState;
		}
		else
		{
			//taskENTER_CRITICAL();
			vTaskSuspendAll();
			TALKER_DDR 	|=   (1 << TALKER_BIT);	// setting talker pin as output
			TALKER_PORT	&=  ~(1 << TALKER_BIT);	// talker output pin should stay low

			_delay_ms(20);

			TALKER_PORT	|=  (1 << TALKER_BIT);	// set pin to high in order to finish the initial handshake

			_delay_us(TIMEOUT_AFTER_HANDSHAKE_US);	//delay task for 40us

			TALKER_DDR 	&= ~(1 << TALKER_BIT);	// setting talker pin as input
			TALKER_PORT	|=  (1 << TALKER_BIT);	// using the internal pull-up

			_delay_us(TIMEOUT_BEFORE_READ_US);	// delay a bit to let the sensor pull the data line low.

			uint32_t pulseCount = getPulseDuration(eLow);
			if (PULSE_COUNT_TIMEDOUT == pulseCount) // expecting a 80us low pulse from DHT
			{
				eRet = ET_NOK;
			}

			uint32_t pulseCount2 = getPulseDuration(eHigh);
			if((eRet == ET_OK) && 								// expecting a 80us high pulse from DHT
			   (PULSE_COUNT_TIMEDOUT == pulseCount2))
			{
				eRet = ET_NOK;
			}

			if(eRet == ET_OK)
			{
				// Now read the 40 bits sent by the sensor.  Each bit is sent as a 50
				// microsecond low pulse followed by a variable length high pulse.  If the
				// high pulse is ~28 microseconds then it's a 0 and if it's ~70 microseconds
				// then it's a 1.

				for (uint8_t u8Loop = 0 ; u8Loop < VOLTAGE_PULSE_COUNT ; u8Loop+=2)
				{
					m_pu8PulseContainer[u8Loop]    = getPulseDuration(eLow);
					m_pu8PulseContainer[u8Loop+1U] = getPulseDuration(eHigh);
				}
			}

			if(eRet == ET_NOK)
			{// in case of measurement issues enter error state
				m_u8NewTalkerState = STATE_ERRORS;
			}
			else
			{// result transfer is completer it is time to evaluate the received values
				m_u8NewTalkerState = STATE_EVALUATING_DATA;
			}

			xTaskResumeAll();
		}

		return (eRet);
	}

	ERRORTYPE DhtTalker::stateEvaluatingData()
	{
		ERRORTYPE eRet = ET_OK;

		// is this the first state entry?
		if(m_u8NewTalkerState != m_u8TalkerState)
		{
			m_u8TalkerState = m_u8NewTalkerState;
		}
		else
		{
		  vTaskSuspendAll();
		  // Inspect pulses and determine which ones are 0 (high state cycle count < low
		  // state cycle count), or 1 (high state cycle count > low state cycle count).
		  for (uint8_t u8Loop = 0 ; u8Loop < (VOLTAGE_PULSE_COUNT/2) ; ++u8Loop)
		  {
			uint8_t u8lowCycles  = m_pu8PulseContainer[2*u8Loop];
			uint8_t u8HighCycles = m_pu8PulseContainer[(2*u8Loop)+1];

			if ((u8lowCycles == PULSE_COUNT_TIMEDOUT) || (u8HighCycles == PULSE_COUNT_TIMEDOUT))
			{
				xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
				xSerialxPrintf_P( &xSerialPort, PSTR("|DHT Talker| Pulse count mismatch %u, system time:%u ms.\r\n"), m_u8TalkerState,xTaskGetAbsolutTimeMs());
				xSemaphoreGive(xConsoleMutex);
				eRet = ET_NOK;
				break;
			}

			m_au8DataBuffer[u8Loop/8] <<= 1; //move previous result

			if (u8HighCycles > u8lowCycles) //comparing the low and high cycle times to see if the bit is a 0 or 1.
			{ // High cycles are greater than 50us low cycle count, must be a 1.
				m_au8DataBuffer[u8Loop/8] |= 1;
			}
			// Else high cycles are less than (or equal to, a weird case) the 50us low
			// cycle count so this must be a zero.
		  }

		  // Check we read 40 bits and that the checksum matches.
		  if ((eRet == ET_OK) &&
			  (m_au8DataBuffer[4] != ((m_au8DataBuffer[0] + m_au8DataBuffer[1] +
				                      m_au8DataBuffer[2] + m_au8DataBuffer[3]) & 0xFF)))
		  {
				xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
				xSerialxPrintf_P( &xSerialPort, PSTR("|DHT Talker| Checksum mismatch %u, system time:%u ms.\r\n"), m_u8TalkerState,xTaskGetAbsolutTimeMs());
				xSemaphoreGive(xConsoleMutex);
		  }

	      if(eRet == ET_NOK)
	      {// in case of measurement issues enter error state
				m_u8NewTalkerState = STATE_ERRORS;
		  }
		  else
		  {// Result evaluation completed prepare for next 1 wire read
				m_u8NewTalkerState = STATE_PRE_INIT;
	      }
	      xTaskResumeAll();
		}

		return (eRet);
	}

	ERRORTYPE DhtTalker::stateError()
	{
		// is this the first state entry?
		if(m_u8NewTalkerState != m_u8TalkerState)
		{
			xSemaphoreTake(xConsoleMutex, portMAX_DELAY);
			xSerialxPrintf_P( &xSerialPort, PSTR("|DHT Talker| Error state, previous state was %u, system time:%u ms.\r\n"), m_u8TalkerState,xTaskGetAbsolutTimeMs());
			xSemaphoreGive(xConsoleMutex);

			m_u8TalkerState = m_u8NewTalkerState;
		}

		return (ET_OK);
	}

	uint32_t DhtTalker::getPulseDuration(VoltagePulseType ePulseType)
	{
		uint32_t u32count = 0;

		while (((TALKER_PIN >> TALKER_BIT) & 1U) == ePulseType)
		{
		  ++u32count;

		  if (u32count >= MAX_PULSE_COUNT )
		  {
			 u32count =  0; // Exceeded timeout.
			 break;
		  }
		}

		return u32count;
	}


} /* namespace sensors */
} /* namespace drivers */
