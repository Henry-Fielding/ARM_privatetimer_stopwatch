/*
 * stopWatchTimer.h
 *
 *  Created on: 22 Mar 2021
 *      Author: Henry
 */

#ifndef STOPWATCHTIMER_H_
#define STOPWATCHTIMER_H_

#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_PrivateTimer/HPS_PrivateTimer.h"

// Define new data type for a functions which takes an int and returns an int
typedef unsigned int (*TaskFunction)(unsigned int);

void configure_privateTimer (void);

void stopWatchTimer_updateTimer (TaskFunction* taskFunctions, unsigned int* time, unsigned int* taskLastTime, long long* taskInterval,  unsigned int taskCount);


unsigned int update_hundredths (unsigned int);


unsigned int update_seconds (unsigned int);


unsigned int update_minutes (unsigned int);


unsigned int update_hours (unsigned int);


#endif /* STOPWATCHTIMER_H_ */
