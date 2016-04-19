/*
 * Globals.h
 *
 *  Created on: 16.04.2016
 *      Author: Marcin.Karczewski
 */

#ifndef OS_GLOBALS_H_
#define OS_GLOBALS_H_

/*
 * \Brief add pre-compiled headers, global variables externs here.
 */

#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <avr/io.h>
#include <avr/sleep.h>
#include <util/delay.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* serial interface include file. */
#include "serial.h"

/* system time include file. */
#include "time.h"

/* holds exact N-bits typedefs mandated by the C99 standard. */
#include "stdint.h"

#include "cppWrappers/TaskCPP.h"

extern QueueHandle_t xConsoleMutex;
extern xComPortHandle xSerialPort;

#endif /* OS_GLOBALS_H_ */
