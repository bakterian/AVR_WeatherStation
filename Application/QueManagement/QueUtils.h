/*
 * QueUtils.h
 *
 *  Created on: 16.04.2016
 *      Author: Marcin.Karczewski
 */

#ifndef APPLICATION_QUEMANAGEMENT_QUEUTILS_H_
#define APPLICATION_QUEMANAGEMENT_QUEUTILS_H_
//#include "commonTypes.h"
#include <stdio.h>
#include <avr/pgmspace.h>

namespace Application
{
namespace QueManagement
{

struct SerialQueItem
{
	PGM_P pgformat;
	va_list vlArgList;

	void print2Serial(PGM_P format, ...)
	{

	}
};

}/* namespace Application */
}/* namespace QueManagement */

#endif /* APPLICATION_QUEMANAGEMENT_QUEUTILS_H_ */
