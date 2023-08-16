#ifndef __POWER_MONITOR_H
#define __POWER_MONITOR_H

#include "main.h"
#include "stdlib.h"

#define AMPS_RATIO		(1.0 / 0.05 / 50)

uint16_t power_monitor_read_value(uint8_t channel);
void power_monitor_init(void);
void power_monitor_deinit(void);


#endif  
	 



