/*
 *	stopWatchTimer (header file)
 *	------------------------------
 *	Created on: 22 Mar 2021
 *	Author: Henry Fielding
 *
 *	Description
 *	-------------
 *	A programme to handle the timer functionality of the stopwatch using a task scheduler
 *	and the private timer moduler
 */

#ifndef STOPWATCHTIMER_H_
#define STOPWATCHTIMER_H_

// Include required header files
#include "HPS_PrivateTimer/HPS_PrivateTimer.h"

// Define new data type for a functions which takes an int and returns an int
typedef unsigned int (*TaskFunction)(unsigned int);

// function to configure the private timer
// initialises to prescaler = 244  and timer to disabled
void configure_privateTimer (void);

// function to update the current stopwatch time
// checks current private timer count and uses task scheduler to update
// hours, minutes, seconds and hundreths at appropriate time intervals
void stopWatchTimer_updateTimer (TaskFunction*, unsigned int*, unsigned int*, const unsigned int*,  unsigned int);

// function to reset the stopwatch timer
// sets time(HH:MM:SS:FF) to 0:0:0:0
// sets task last time for each task to current time
void stopWatchTimer_resetTimer (unsigned int*, unsigned int*, unsigned int);

// function to save current stopwatch timer value to the split timer array
void stopWatchTimer_splitTimer (unsigned int*, unsigned int*, unsigned int);

// task scheduler function to update the hundredths of seconds column
unsigned int update_hundredths (unsigned int);

// task scheduler function to update the seconds column
unsigned int update_seconds (unsigned int);

// task scheduler function to update the timer minutes column
unsigned int update_minutes (unsigned int);

// task scheduler function to update the timer hours column
unsigned int update_hours (unsigned int);

#endif /* STOPWATCHTIMER_H_ */
