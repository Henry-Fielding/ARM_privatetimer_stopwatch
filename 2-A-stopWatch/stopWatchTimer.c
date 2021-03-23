/*
 *	stopWatchTimer (source file)
 *	------------------------------
 *	Created on: 22 Mar 2021
 *	Author: Henry Fielding
 *
 *	Description
 *	-------------
 *	A programme to handle the timer functionality of the stopwatch using a task scheduler
 *	and the private timer moduler
 */

#include "stopWatchTimer.h"

// function to configure the private timer
// initialises to prescaler = 224  and timer to disabled
void configure_privateTimer () {
	Timer_initialise(0xFFFEC600);	// set private timer base address
	Timer_setLoadValue(0xFFFFFFFF);	// load maximum value
	Timer_setControl(224, 0, 1, 0);	// timer initialised to disabled mode
}

// function to update the current stopwatch time
// checks current private timer count and uses task scheduler to update
// hours, minutes, seconds and hundreths at appropriate time intervals
void stopWatchTimer_updateTimer (TaskFunction* taskFunctions, unsigned int* time, unsigned int* taskLastTime, const unsigned int* taskInterval,  unsigned int taskCount) {
	unsigned int taskID;
	unsigned int currentTimerValue = Timer_readTimer();

	for (taskID = 0; taskID < taskCount; taskID++) {
		// run each task if time since last run is >= the task interval.
		if ((taskLastTime[taskID] - currentTimerValue) >= taskInterval[taskID]) {
			time[taskID] = taskFunctions[taskID](time[taskID]);
			taskLastTime[taskID] = taskLastTime[taskID] - taskInterval[taskID];	// update task last time using task interval to avoid compounding error
		}
	}
}

// function to reset the stopwatch timer
// sets time(HH:MM:SS:FF) to 0:0:0:0
// sets task last time for each task to current time
void stopWatchTimer_resetTimer (unsigned int* time, unsigned int* taskLastTime, unsigned int arrayLength) {
	unsigned int i;

	for (i = 0; i < arrayLength; i++) {
		time[i] = 0;							// all time columns = 0
		taskLastTime[i] = Timer_readTimer();	// all tasks start now
	}
}

// function to save current stopwatch timer value to the split timer array
void stopWatchTimer_splitTimer (unsigned int* time, unsigned int* splitTime, unsigned int arrayLength) {
	unsigned int i;

	for (i = 0; i < arrayLength; i++) {
		splitTime[i] = time[i];
	}
}

// task scheduler function to update the hundredths of seconds column
unsigned int update_hundredths (unsigned int hundredths) {
	if (hundredths < 99) hundredths = hundredths + 1;
	else hundredths = 0;

	return hundredths;
}

// task scheduler function to update the seconds column
unsigned int update_seconds (unsigned int seconds) {
	if (seconds < 59) seconds = seconds + 1;
	else seconds = 0;

	return seconds;
}

// task scheduler function to update the timer minutes column
unsigned int update_minutes (unsigned int minutes) {
	if (minutes < 59) minutes = minutes + 1;
	else minutes = 0;

	return minutes;
}

// task scheduler function to update the timer hours column
unsigned int update_hours (unsigned int hours) {
	if (hours < 99) hours = hours + 1;
	else hours = 0;

	return hours;
}
