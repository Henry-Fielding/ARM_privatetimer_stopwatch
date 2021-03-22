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

#ifndef STOPWATCH_MAIN_H_
#define STOPWATCH_MAIN_H_

// Include required header files
#include "DE1Soc_SevenSeg/DE1Soc_SevenSeg.h"
#include "DE1SoC_Servo/DE1SoC_Servo.h"
#include "stopWatchTimer.h"

// hardware base address definitions
volatile unsigned int *key_ptr = (unsigned int *)0xFF200050; // KEYS 0-3 (push buttons)
volatile unsigned int *LEDR_ptr = (unsigned int *)0xFF200000; // LEDS 0-9
volatile unsigned int *key_edge_ptr = (unsigned int *)0xFF20005C;

// stopwatch statemachine state definitions
#define RESET_STATE 0
#define STOP_STATE 1
#define START_STATE 2
#define SPLIT_STATE 3

// stopwatch split type defintions
#define CUMULATIVE_SPLIT 0
#define LAP_SPLIT 1

// function to configure servo drivers for servo 0
// intialises servo 0 and sets position to centre
void configure_servoDrivers (void);

// function to update the statemachine
// - RESET_STATE set the stopwatch time to 0 and disables the private timer
// - STOP_STATE disables the private timer and displays the current stopwatch time
// - START_STATE enables the private timer and displays the current stopwatch time
// - SPLIT_STATE enables the private timer and displays the split time (resets clock if lap split mode but not in cummulative split mode)
void stopWatch_updateStateMachine (TaskFunction*, unsigned int*, unsigned int*, unsigned int*, unsigned int*, unsigned int*, const unsigned int*,  unsigned int);

// function to update split mode
// switches split mode when split button is pressed
void stopWatch_updateSplitMode (unsigned int*);

// function to display time on 7 seg LEDs
// if time < 1hr displays time in form MM:SS:FF (also displays hundredths indicator)
// if time > 1hr displays time in form HH:MM:SS
void stopWatch_displayTime (unsigned int*);

// function to update metronome
// metronome position alternates every second
void stopWatch_updateMetronome (unsigned int*);

// function to clear inputs
// clears all set bits in the key edge register by writing 1 to them
void stopWatch_clearInputs (void);

#endif /* STOPWATCH_MAIN_H_ */
