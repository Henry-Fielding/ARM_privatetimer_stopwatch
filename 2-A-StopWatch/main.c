/*
 * Digital Stopwatch
 *-------------------------------------------
 * 	Created on: 18 Mar 2021
 *	Author: Henry
 *
 *	Description
 *	------------
 *	This program uses a simple task scheduler and the private timer registers to
 *	perform the functions of a basic stopwatch and display the output on the 7 segment LEDS
 *
 */

// Include required drivers:
#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_PrivateTimer/HPS_PrivateTimer.h"
#include "DE1Soc_SevenSeg/DE1Soc_SevenSeg.h"

volatile unsigned int *key_ptr = (unsigned int *)0xFF200050; // KEYS 0-3 (push buttons)
volatile unsigned int *LEDR_ptr = (unsigned int *)0xFF200000; // LEDS 0-9


// Define new data type for a functions which takes an int and returns an int
typedef unsigned int (*TaskFunction)(unsigned int, unsigned int);

// Declare functions
unsigned int update_hundredths (unsigned int hours, unsigned int hundredths) {
	if (hundredths < 99) hundredths = hundredths + 1;
	else hundredths = 0;

	if (hours < 1) DE1SoC_SevenSeg_SetDoubleDec(0, hundredths);

	return hundredths;
}

unsigned int update_seconds (unsigned int hours, unsigned int seconds) {
	if (seconds < 59) seconds = seconds + 1;
	else seconds = 0;

	if (hours < 1) DE1SoC_SevenSeg_SetDoubleDec(2, seconds);
	else DE1SoC_SevenSeg_SetDoubleDec(0, seconds);

	return seconds;
}

unsigned int update_minutes (unsigned int hours, unsigned int minutes) {
	if (minutes < 59) minutes = minutes + 1;
	else minutes = 0;

	if (hours < 1) DE1SoC_SevenSeg_SetDoubleDec(4, minutes);
	else DE1SoC_SevenSeg_SetDoubleDec(2, minutes);

	return minutes;
}

unsigned int update_hours (unsigned int hours, unsigned int unused) {
	if (hours < 99) hours = hours + 1;
	else hours = 0;

	if (hours > 0) DE1SoC_SevenSeg_SetDoubleDec(4, hours);

	return hours;
}

main() {
	// local variables
	const unsigned int taskCount = 4;
	unsigned int taskID = 0;
	unsigned int taskLastTime[taskCount];
	unsigned int taskInterval[taskCount] = {10000, 1000000, 60000000, 3600000000};
	TaskFunction taskFunctions[taskCount] = {&update_hundredths, &update_seconds, &update_minutes, &update_hours};

	unsigned int time[taskCount] = {0, 0, 0, 0};

	// configure ARM private timer
	Timer_initialise(0xFFFEC600);
	Timer_setLoadValue(0xFFFFFFFF);
	Timer_setControl(224, 0, 1, 1);

	for (taskID = 0; taskID < taskCount; taskID++) {
		taskLastTime[taskID] = Timer_readTimer();      //All tasks start now
	}

	// set intial task times
//	taskLastTime[0] = Timer_readTimer();		//All tasks start now
//	for (taskID = 1; taskID < taskCount+1; taskID++) {
//		taskLastTime[taskID] = 0;							//All timer storage start at 0
//	}

	while(1) {
		// Read the current time
		unsigned int currentTimerValue = Timer_readTimer();

		if(*key_ptr & 0x01){
			Timer_setControl(224, 0, 1, 1);
			*LEDR_ptr = 0x01;
		} else if(*key_ptr & 0x02){
			Timer_setControl(224, 0, 1, 0);
			*LEDR_ptr = 0x02;
		} else *LEDR_ptr = 0x00;

		for (taskID = 0; taskID < taskCount; taskID++) {
			if ((taskLastTime[taskID] - currentTimerValue) >= taskInterval[taskID]) {
				time[taskID] = taskFunctions[taskID](time[3], time[taskID]);
				taskLastTime[taskID] = taskLastTime[taskID] - taskInterval[taskID];
			}
		}

		// Make sure we clear the private timer interrupt flag if it is set
		if (Timer_readInterrupt() & 0x1) {
			// If the timer interrupt flag is set, clear the flag
			Timer_clearInterrupt();
		}

		// Finally, reset the watchdog timer.
		HPS_ResetWatchdog();
	}


}
