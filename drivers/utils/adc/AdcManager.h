/*
 * AdcManager.h
 *
 *  Created on: 18.06.2016
 *      Author: Marcin.Karczewski
 */

#ifndef DRIVERS_UTILS_ADC_ADCMANAGER_H_
#define DRIVERS_UTILS_ADC_ADCMANAGER_H_

namespace drivers {
namespace sensors {

class AdcManager {
public:
	AdcManager();
	virtual ~AdcManager();
};

} /* namespace sensors */
} /* namespace drivers */

#endif /* DRIVERS_UTILS_ADC_ADCMANAGER_H_ */
