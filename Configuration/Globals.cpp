/*
 * Globals.cpp
 *
 *  Created on: 16.04.2016
 *      Author: Marcin.Karczewski
 */

/* REMEMBER TO AVOID REFERNECING THIS GLOBAL VARIABLES OTHER THAN IN MAIN
 * IT SAVES RAM IF YOU PASS THE DATA VIA THE CONSTRUCTOR */



#include "Globals.h"

QueueHandle_t xConsoleMutex;
xComPortHandle xSerialPort;

/* new and delete operator definitions, which use the RTOS memory allocation
 * AVR-GCC is missing libstdc++ and other C++ features therefore new and delete operators were not implemented.
 */


// Define the 'new' operator for C++ to use the freeRTOS memory management
// functions. THIS IS NOT OPTIONAL!
//
void *operator new(size_t size){
	void *p=pvPortMalloc(size);
#ifdef	__EXCEPTIONS
	if (p==0) // did pvPortMalloc succeed?
		throw std::bad_alloc(); // ANSI/ISO compliant behavior
#endif
	return p;
}

//
// Define the 'delete' operator for C++ to use the freeRTOS memory management
// functions. THIS IS NOT OPTIONAL!
//
void operator delete(void *p){
	vPortFree( p );
}


int __cxa_atexit(void (*destructor) (void *), void *arg, void *dso)
{
    return 0;
}

void __cxa_finalize(void *f)
{

}

int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {};

void __cxa_pure_virtual(void) {};

uint16_t  xTaskGetAbsolutTimeMs()
{
	return xTaskGetTickCount() * portTICK_PERIOD_MS;
}

