/*
 * stopWatchTimer.c
 *
 *  Created on: 22 Mar 2021
 *      Author: Henry
 */

#include "stopWatchTimer.h"

//	const unsigned int taskCount = 4;
//	unsigned int taskID = 0;
//	unsigned int taskLastTime[taskCount];
//	long long taskInterval[taskCount] = {10000, 1000000, 60000000, 3600000000};
//	TaskFunction taskFunctions[taskCount] = {&update_hundredths, &update_seconds, &update_minutes, &update_hours};



void configure_privateTimer () {
	Timer_initialise(0xFFFEC600);	// set private timer base address
	Timer_setLoadValue(0xFFFFFFFF);	// load maximum value
	Timer_setControl(224, 0, 1, 0);	// timer intialised to disabled mode
}

void stopWatchTimer_updateTimer (TaskFunction* taskFunctions, unsigned int* time, unsigned int* taskLastTime, long long* taskInterval,  unsigned int taskCount) {
	unsigned int taskID;
	unsigned int currentTimerValue = Timer_readTimer();

	for (taskID = 0; taskID < taskCount; taskID++) {
		if ((taskLastTime[taskID] - currentTimerValue) >= taskInterval[taskID]) {
			time[taskID] = taskFunctions[taskID](time[taskID]);
			taskLastTime[taskID] = taskLastTime[taskID] - taskInterval[taskID];
		}
	}
}

void stopWatchTimer_resetTimer (unsigned int* time, unsigned int* taskLastTime, unsigned int arrayLength) {
	unsigned int i;

	for (i = 0; i < arrayLength; i++) {
		time[i] = 0;
		taskLastTime[i] = Timer_readTimer();      //All tasks start now
	}
}

void stopWatchTimer_splitTimer (unsigned int* time, unsigned int* splitTime, unsigned int arrayLength) {
	unsigned int i;

	for (i = 0; i < arrayLength; i++) {
		splitTime[i] = time[i];
	}
}


//
// Declare task scheduler functions
//
unsigned int update_hundredths (unsigned int hundredths) {
	if (hundredths < 99) hundredths = hundredths + 1;
	else hundredths = 0;

	return hundredths;
}

unsigned int update_seconds (unsigned int seconds) {
	signed char position; // calibration performed manually as calibration function does not work
	signed char calibration = 20;

	if (seconds < 59) seconds = seconds + 1;
	else seconds = 0;

	if (seconds % 2) position = 64 + calibration;
	else position = -64 + calibration;

	Servo_pulseWidth(0, position);

	return seconds;
}

unsigned int update_minutes (unsigned int minutes) {
	if (minutes < 59) minutes = minutes + 1;
	else minutes = 0;

	return minutes;
}

unsigned int update_hours (unsigned int hours) {
	if (hours < 99) hours = hours + 1;
	else hours = 0;

	return hours;
}
