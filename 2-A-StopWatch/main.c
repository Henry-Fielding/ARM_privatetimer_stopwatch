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

#include "main.h"

main() {
	// taskscheduler variables
	int temp;
	int state = 0;

	bool displayMode = 0;
	int splitMode = 0;

	const unsigned int taskCount = 4;
	unsigned int taskID = 0;
	unsigned int taskLastTime[taskCount];
	long long taskInterval[taskCount] = {10000, 1000000, 60000000, 3600000000};
	TaskFunction taskFunctions[taskCount] = {&update_hundredths, &update_seconds, &update_minutes, &update_hours};

	unsigned int currentTimerValue;
	unsigned int time[taskCount] = {0};
	unsigned int splitTime[taskCount] = {0};

	#define RESET_STATE 0
	#define STOP_STATE 1
	#define START_STATE 2
	#define SPLIT_STATE 3

	#define CUMMULATIVE_SPLIT 0
	#define LAP_SPLIT 1



	//configure drivers
	configure_privateTimer();
	configure_servoDrivers();

	// Begin task timer now
	reset_stopWatch(time, taskLastTime, taskCount);

	while(1) {
		// poll keys to check for user inputs
//		if(*key_edge_ptr & 0x01) {						//enable timer when start pressed
//			if(state == 0) {
//				// stop state
//				state = 1;
//				Timer_setControl(224, 0, 1, 1);
//				*LEDR_ptr &= ~0x01;
//			} else {
//				// start state
//				state = 0;
//				Timer_setControl(224, 0, 1, 0);
//				*LEDR_ptr |= 0x01;
//			}
//		}
//		else if(*key_edge_ptr &0x04) {
//			if(state == 0) {
//				reset_stopWatch(time, taskLastTime, taskCount);	//reset time when rest pressed
//			} else {
//
//
//			}
//		}
//		temp = *key_edge_ptr;
//		*key_edge_ptr = temp;
//

		switch (state) {
			case RESET_STATE :
				Timer_setControl(224, 0, 1, 0);
				reset_stopWatch(time, taskLastTime, taskCount);

				displayMode = 0;

				state = STOP_STATE;
				break;

			case STOP_STATE :
				Timer_setControl(224, 0, 1, 0);
				displayMode = 0;
				*LEDR_ptr |= 0x001;

				if (*key_edge_ptr & 0x02) update_split_mode_stopWatch(&splitMode);

				if(*key_edge_ptr & 0x01) state = START_STATE;
				else if(*key_edge_ptr & 0x04) state = RESET_STATE;
				else state = STOP_STATE;

				break;

			case START_STATE :
				displayMode = 0;
				Timer_setControl(224, 0, 1, 1);
				*LEDR_ptr &= ~0x001;

				if(*key_edge_ptr & 0x01) state = STOP_STATE;
				else if(*key_edge_ptr & 0x04) {
					state = SPLIT_STATE;
					split_stopWatch(time, splitTime, taskCount);
					if (splitMode == LAP_SPLIT) reset_stopWatch(time, taskLastTime, taskCount);
				} else state = START_STATE;

				break;

			case SPLIT_STATE :
				displayMode = 1;
				Timer_setControl(224, 0, 1, 1);

				if(*key_edge_ptr & 0x04) state = START_STATE;
				else state = SPLIT_STATE;

				break;

		}

		temp = *key_edge_ptr;
		*key_edge_ptr = temp;






		// Read the current time
		currentTimerValue = Timer_readTimer();

		for (taskID = 0; taskID < taskCount; taskID++) {
			if ((taskLastTime[taskID] - currentTimerValue) >= taskInterval[taskID]) {
				time[taskID] = taskFunctions[taskID](time[taskID]);
				taskLastTime[taskID] = taskLastTime[taskID] - taskInterval[taskID];
			}
		}

		if (displayMode == 1) display_stopWatch(splitTime);
		else display_stopWatch(time);

		// Make sure we clear the private timer interrupt flag if it is set
		if (Timer_readInterrupt() & 0x1) {
			// If the timer interrupt flag is set, clear the flag
			Timer_clearInterrupt();
		}

		// Finally, reset the watchdog timer.
		HPS_ResetWatchdog();
	}


}


void configure_privateTimer () {
	Timer_initialise(0xFFFEC600);	// set private timer base address
	Timer_setLoadValue(0xFFFFFFFF);	// load maximum value
	Timer_setControl(224, 0, 1, 0);	// timer intialised to disabled mode

}

void configure_servoDrivers () {
	//signed char calibrate = -100;
	Servo_initialise(0xFF2000C0);
	Servo_enable(0, true);
	Servo_pulseWidthRange(0, true);

	// Servo calibrate does not seem to work
	//Servo_calibrate(0, calibrate);

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

void reset_stopWatch (unsigned int* time, unsigned int* taskLastTime, unsigned int arrayLength) {
	unsigned int i;

//	Timer_setControl(224, 0, 1, 0);
	for (i = 0; i < arrayLength; i++) {
		time[i] = 0;
		taskLastTime[i] = Timer_readTimer();      //All tasks start now
	}

	DE1SoC_SevenSeg_SetDoubleDec(0, 0);
	DE1SoC_SevenSeg_SetDoubleDec(2, 0);
	DE1SoC_SevenSeg_SetDoubleDec(4, 0);
}

void split_stopWatch (unsigned int* time, unsigned int* splitTime, unsigned int arrayLength) {
	unsigned int i;

	for (i = 0; i < arrayLength; i++) {
		splitTime[i] = time[i];
	}
}

void display_stopWatch (unsigned int* time) {
	unsigned int hundredths = time[0];
	unsigned int seconds = time[1];
	unsigned int minutes = time[2];
	unsigned int hours = time[3];

	if (hours <= 0) {	// if time is less than 1 hours show MM:SS:FF, show hundredths indicator LED
		*LEDR_ptr |= 0x200;
		DE1SoC_SevenSeg_SetDoubleDec(0, hundredths);
		DE1SoC_SevenSeg_SetDoubleDec(2, seconds);
		DE1SoC_SevenSeg_SetDoubleDec(4, minutes);
	} else {	// if time is greater than 1 hours show HH:MM:SS
		*LEDR_ptr &= ~0x200;
		DE1SoC_SevenSeg_SetDoubleDec(0, seconds);
		DE1SoC_SevenSeg_SetDoubleDec(2, minutes);
		DE1SoC_SevenSeg_SetDoubleDec(4, hours);
	}


}


void update_split_mode_stopWatch (int* splitMode) {
	if (*splitMode == CUMMULATIVE_SPLIT) {
		*splitMode = LAP_SPLIT;
		*LEDR_ptr &= ~0x004;
		*LEDR_ptr |= 0x002;
	} else if (*splitMode == LAP_SPLIT) {
		*splitMode = CUMMULATIVE_SPLIT;
		*LEDR_ptr &= ~0x002;
		*LEDR_ptr |= 0x004;
	}
}
