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

// Define new data type for a functions which takes an int and returns an int
typedef unsigned int (*TaskFunction)(unsigned int);

// Declare functions
unsigned int update_hundredths (unsigned int hundredths) {
	if (hundredths > 99) hundredths = 1;
	else hundredths = hundredths + 1;
	DE1SoC_SevenSeg_SetDoubleDec(0, hundredths);

	return hundredths;
}

main() {
	// local variables
	const unsigned int taskCount = 1;
	unsigned int taskID = 0;
	unsigned int taskLastTime[taskCount];
	unsigned int taskInterval[taskCount] = {2250000};
	TaskFunction taskFunctions[taskCount] = {&update_hundredths};

	// current timer storage variable
	unsigned int time[taskCount];

	// set intial task times
	for (taskID = 0; taskID < taskCount; taskID++) {
		taskLastTime[taskID] = Timer_readTimer();      //All tasks start now
	}

	// configure ARM private timer
	Timer_initialise(0xFFFEC600);
	Timer_setLoadValue(0xFFFFFFFF);
	Timer_setControl(0, 0, 1, 1);

	while(1) {
		// Read the current time
		unsigned int currentTimerValue = Timer_readTimer();

		for (taskID = 0; taskID < taskCount; taskID++) {
			if ((taskLastTime[taskID] - currentTimerValue) >= taskInterval[taskID]) {
				time[taskID] = taskFunctions[taskID](time[taskID]);

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
