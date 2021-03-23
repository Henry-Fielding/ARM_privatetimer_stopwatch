/*
 *	Digital Stopwatch (source file)
 *	-------------------------------------------
 *	Created on: 18 Mar 2021
 *	Author: Henry Fielding
 *
 *	Description
 *	-------------
 *	Program to handle the top level functionality of the stopwatch using a statemachine
 *	taking inputs from keys [2:0] and outputting to 7 segment LEDS, red LEDs, and servo 0
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


// function to configure servo drivers for servo 0
// intialises servo 0 and sets position to centre
void configure_servoDrivers () {
	signed char calibrate = 20;
	Servo_initialise(0xFF2000C0);
	Servo_calibrate(0, calibrate); 	// set calibration offset (does not work)

	Servo_enable(0, true); 			// enable servo 0
	Servo_pulseWidthRange(0, true); // set servo 0 position to centre
}

// function to update the statemachine
// - RESET_STATE set the stopwatch time to 0 and disables the private timer
// - STOP_STATE disables the private timer and displays the current stopwatch time
// - START_STATE enables the private timer and displays the current stopwatch time
// - SPLIT_STATE enables the private timer and displays the split time (resets clock if lap split mode but not in cummulative split mode)
void stopWatch_updateStateMachine (TaskFunction* taskFunctions, unsigned int* state_ptr, unsigned int* splitMode_ptr, unsigned int* time, unsigned int* splitTime, unsigned int* taskLastTime, const unsigned int* taskInterval,  unsigned int taskCount) {
	switch (*state_ptr) {
		case RESET_STATE :
			// pause and reset timer
			Timer_setControl(224, 0, 1, 0);
			stopWatchTimer_resetTimer(time, taskLastTime, taskCount);

			// change state conditions
			*state_ptr = STOP_STATE;
		break;

		case STOP_STATE :
			// pause timer and update split mode
			Timer_setControl(224, 0, 1, 0);
			stopWatch_updateSplitMode(splitMode_ptr);

			// set stopwatch outputs
			stopWatch_displayTime(time);
			stopWatch_updateMetronome (time);
			*LEDR_ptr |= 0x001;		// display stopped indicator

			// change state conditions
			if(*key_edge_ptr & 0x01) {
				stopWatch_clearInputs();
				*state_ptr = START_STATE;
			} else if(*key_edge_ptr & 0x04) {
				stopWatch_clearInputs();
				*state_ptr = RESET_STATE;
			} else {
				*state_ptr = STOP_STATE;
			}
		break;

		case START_STATE :
			// run timer
			Timer_setControl(224, 0, 1, 1);

			// set stopwatch outputs
			stopWatch_displayTime(time);
			stopWatch_updateMetronome (time);
			*LEDR_ptr &= ~0x009;	// turn off split and stopped indicators

			// change state conditions
			if(*key_edge_ptr & 0x01) {
				stopWatch_clearInputs();
				*state_ptr = STOP_STATE;
			} else if(*key_edge_ptr & 0x04) {
				stopWatch_clearInputs();
				stopWatchTimer_splitTimer(time, splitTime, taskCount);	// save split value
				if (*splitMode_ptr == LAP_SPLIT) {
					// reset stopwatch timer if in lap split mode
					stopWatchTimer_resetTimer(time, taskLastTime, taskCount);
				}
				*state_ptr = SPLIT_STATE;
			} else {
				*state_ptr = START_STATE;
			}

		break;

		case SPLIT_STATE :
			// run timer
			Timer_setControl(224, 0, 1, 1);

			// set stopwatch outputs
			stopWatch_displayTime(splitTime); 	// display split value
			*LEDR_ptr |= 0x008; 				// display split indicator

			// change state conditions
			if(*key_edge_ptr & 0x04) {
				stopWatch_clearInputs ();
				*state_ptr = START_STATE;
			} else {
				*state_ptr = SPLIT_STATE;
			}
		break;
	}
}

// function to update split mode
// switches split mode when split button is pressed
void stopWatch_updateSplitMode (unsigned int* splitMode_ptr) {
	switch (*splitMode_ptr) {
		case CUMULATIVE_SPLIT :
			*LEDR_ptr &= ~0x004;	// set Cumulative indicator
			*LEDR_ptr |= 0x002;

			// change state conditions
			if (*key_edge_ptr & 0x02) {
				*splitMode_ptr = LAP_SPLIT;
				stopWatch_clearInputs ();
			}
		break;

		case LAP_SPLIT :
			*LEDR_ptr &= ~0x002; // set lap indicator
			*LEDR_ptr |= 0x004;

			// change state conditions
			if (*key_edge_ptr & 0x02) {
				*splitMode_ptr = CUMULATIVE_SPLIT;
				stopWatch_clearInputs ();
			}
		break;
	}
}

// function to display time on 7 seg LEDs
// if time < 1hr displays time in form MM:SS:FF (also displays hundredths indicator)
// if time > 1hr displays time in form HH:MM:SS
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
	} else {		// if time is greater than 1 hours show HH:MM:SS
		*LEDR_ptr &= ~0x010;
		DE1SoC_SevenSeg_SetDoubleDec(0, seconds);
		DE1SoC_SevenSeg_SetDoubleDec(2, minutes);
		DE1SoC_SevenSeg_SetDoubleDec(4, hours);
	}
}

// function to update metronome
// metronome position alternates every second
void stopWatch_updateMetronome (unsigned int* time) {
	unsigned int seconds = time[1];
	signed char position;
	signed char calibration = 20;	// calibration performed manually as calibration function does not work

	if (seconds % 2) position = 64 + calibration;	// alternate servo position every second
	else position = -64 + calibration;

	Servo_pulseWidth(0, position);
}

// function to clear inputs
// clears all set bits in the key edge register by writing 1 to them
void stopWatch_clearInputs () {
	unsigned int temp = *key_edge_ptr;
	*key_edge_ptr = temp;
}
