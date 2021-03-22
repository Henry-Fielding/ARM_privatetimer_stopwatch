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
	// declare task scheduler variables
	const unsigned int taskCount = 4;
	const unsigned int period = 10000;
	const unsigned int taskInterval[taskCount] = {period, 100*period, 6000*period, 360000*period};
	unsigned int taskLastTime[taskCount];
	TaskFunction taskFunctions[taskCount] = {&update_hundredths, &update_seconds, &update_minutes, &update_hours};

	// declare stopwatch variables
	unsigned int time[taskCount] = {0};
	unsigned int splitTime[taskCount] = {0};

	// declare state variables
	unsigned int state = RESET_STATE;
	unsigned int splitMode = CUMULATIVE_SPLIT;


	//configure drivers
	configure_privateTimer();
	configure_servoDrivers();

	// Begin task timer now
	//stopWatchTimer_resetTimer(time, taskLastTime, taskCount);

	while(1) {

		// update the stopwatch timer value
		stopWatchTimer_updateTimer (taskFunctions, time, taskLastTime, taskInterval, taskCount);

		// update stopwatch output mode
		stopWatch_updateStateMachine (taskFunctions, &state, &splitMode, time, splitTime, taskLastTime, taskInterval, taskCount);

		// clear user button inputs
		//stopWatch_clearInputs();

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

void stopWatch_updateStateMachine (TaskFunction* taskFunctions, unsigned int* state, unsigned int* splitMode, unsigned int* time, unsigned int* splitTime, unsigned int* taskLastTime, const unsigned int* taskInterval,  unsigned int taskCount) {
	switch (*state) {
		case RESET_STATE :
			Timer_setControl(224, 0, 1, 0);
			stopWatchTimer_resetTimer(time, taskLastTime, taskCount);

			stopWatch_displayTime(time);
			stopWatch_updateMetronome (time);

			*state = STOP_STATE;
		break;

		case STOP_STATE :
			Timer_setControl(224, 0, 1, 0);
			stopWatch_updateSplitMode(splitMode);

			stopWatch_displayTime(time);
			stopWatch_updateMetronome (time);
			*LEDR_ptr |= 0x001;

			// change state conditions
			if(*key_edge_ptr & 0x01) {
				stopWatch_clearInputs();
				*state = START_STATE;
			} else if(*key_edge_ptr & 0x04) {
				stopWatch_clearInputs();
				*state = RESET_STATE;
			} else {
				*state = STOP_STATE;
			}
		break;

		case START_STATE :
			Timer_setControl(224, 0, 1, 1);

			stopWatch_displayTime(time);
			stopWatch_updateMetronome (time);
			*LEDR_ptr &= ~0x009;

			// change state conditions
			if(*key_edge_ptr & 0x01) {
				stopWatch_clearInputs();
				*state = STOP_STATE;
			} else if(*key_edge_ptr & 0x04) {
				stopWatch_clearInputs();
				stopWatchTimer_splitTimer(time, splitTime, taskCount);
				if (*splitMode == LAP_SPLIT) {
					stopWatchTimer_resetTimer(time, taskLastTime, taskCount);
				}
				*state = SPLIT_STATE;
			} else {
				*state = START_STATE;
			}

		break;

		case SPLIT_STATE :
			Timer_setControl(224, 0, 1, 1);

			stopWatch_displayTime(splitTime);
			*LEDR_ptr |= 0x008;

			// change state conditions
			if(*key_edge_ptr & 0x04) {
				stopWatch_clearInputs ();
				*state = START_STATE;
			} else {
				*state = SPLIT_STATE;
			}
		break;

	}

}

void stopWatch_updateSplitMode (unsigned int* splitMode) {
	switch (*splitMode) {
		case CUMULATIVE_SPLIT :
			*LEDR_ptr &= ~0x004;
			*LEDR_ptr |= 0x002;

			if (*key_edge_ptr & 0x02) {
				*splitMode = LAP_SPLIT;
				stopWatch_clearInputs ();
			}
		break;

		case LAP_SPLIT :
			*LEDR_ptr &= ~0x002;
			*LEDR_ptr |= 0x004;

			if (*key_edge_ptr & 0x02) {
				*splitMode = CUMULATIVE_SPLIT;
				stopWatch_clearInputs ();
			}
		break;
	}
}

void stopWatch_displayTime (unsigned int* time) {
	unsigned int hundredths = time[0];
	unsigned int seconds = time[1];
	unsigned int minutes = time[2];
	unsigned int hours = time[3];

	if (hours <= 0) {	// if time is less than 1 hours show MM:SS:FF, show hundredths indicator LED
		*LEDR_ptr |= 0x010;
		DE1SoC_SevenSeg_SetDoubleDec(0, hundredths);
		DE1SoC_SevenSeg_SetDoubleDec(2, seconds);
		DE1SoC_SevenSeg_SetDoubleDec(4, minutes);
	} else {	// if time is greater than 1 hours show HH:MM:SS
		*LEDR_ptr &= ~0x010;
		DE1SoC_SevenSeg_SetDoubleDec(0, seconds);
		DE1SoC_SevenSeg_SetDoubleDec(2, minutes);
		DE1SoC_SevenSeg_SetDoubleDec(4, hours);
	}
}

void stopWatch_updateMetronome (unsigned int* time) {
	unsigned int seconds = time[1];
	signed char position;
	signed char calibration = 20; // calibration performed manually as calibration function does not work

	if (seconds % 2) position = 64 + calibration;
	else position = -64 + calibration;

	Servo_pulseWidth(0, position);
}

void stopWatch_clearInputs () {
	unsigned int temp = *key_edge_ptr;
	*key_edge_ptr = temp;
}







