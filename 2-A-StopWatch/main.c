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

	int splitMode = 0;

	const unsigned int taskCount = 4;
	unsigned int taskLastTime[taskCount];
	long long taskInterval[taskCount] = {10000, 1000000, 60000000, 3600000000};
	TaskFunction taskFunctions[taskCount] = {&update_hundredths, &update_seconds, &update_minutes, &update_hours};

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
	stopWatchTimer_resetTimer(time, taskLastTime, taskCount);

	while(1) {

		// update the stopwatch timer value
		stopWatchTimer_updateTimer (taskFunctions, time, taskLastTime, taskInterval, taskCount);

		// update stopwatch mode
		switch (state) {
			case RESET_STATE :
				Timer_setControl(224, 0, 1, 0);
				stopWatchTimer_resetTimer(time, taskLastTime, taskCount);

				display_stopWatch(time);

				state = STOP_STATE;
				break;

			case STOP_STATE :
				Timer_setControl(224, 0, 1, 0);
				display_stopWatch(time);
				*LEDR_ptr |= 0x001;

				if (*key_edge_ptr & 0x02) update_split_mode_stopWatch(&splitMode);

				if(*key_edge_ptr & 0x01) state = START_STATE;
				else if(*key_edge_ptr & 0x04) state = RESET_STATE;
				else state = STOP_STATE;

				break;

			case START_STATE :
				display_stopWatch(time);
				Timer_setControl(224, 0, 1, 1);
				*LEDR_ptr &= ~0x001;

				if(*key_edge_ptr & 0x01) state = STOP_STATE;
				else if(*key_edge_ptr & 0x04) {
					state = SPLIT_STATE;
					stopWatchTimer_splitTimer(time, splitTime, taskCount);
					if (splitMode == LAP_SPLIT) stopWatchTimer_resetTimer(time, taskLastTime, taskCount);
				} else state = START_STATE;

				break;

			case SPLIT_STATE :
				display_stopWatch(splitTime);
				Timer_setControl(224, 0, 1, 1);

				if(*key_edge_ptr & 0x04) state = START_STATE;
				else state = SPLIT_STATE;

				break;

		}

		temp = *key_edge_ptr;
		*key_edge_ptr = temp;






		// Make sure we clear the private timer interrupt flag if it is set
		if (Timer_readInterrupt() & 0x1) {
			// If the timer interrupt flag is set, clear the flag
			Timer_clearInterrupt();
		}

		// Finally, reset the watchdog timer.
		HPS_ResetWatchdog();
	}


}



void configure_servoDrivers () {
	signed char calibrate = 20;
	Servo_initialise(0xFF2000C0);
	Servo_calibrate(0, calibrate);

	Servo_enable(0, true);
	Servo_pulseWidthRange(0, true);

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
