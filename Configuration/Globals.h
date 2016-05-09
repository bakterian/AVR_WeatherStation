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
#include "commonTypes.h"

/* serial interface include file. */
#include "serial.h"

/* system time include file. */
#include "time.h"

/* holds exact N-bits typedefs mandated by the C99 standard. */
#include "stdint.h"

#include "cppWrappers/TaskCPP.h"

__extension__ typedef int __guard __attribute__((mode (__DI__)));


extern "C" int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso);
extern "C" void __cxa_finalize(void *f);

extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);

extern "C" void __cxa_pure_virtual(void);

extern QueueHandle_t xConsoleMutex;
extern xComPortHandle xSerialPort;

#endif /* OS_GLOBALS_H_ */
