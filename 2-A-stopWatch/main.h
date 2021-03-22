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
//#include "HPS_Watchdog/HPS_Watchdog.h"
//#include "HPS_PrivateTimer/HPS_PrivateTimer.h"
#include "DE1Soc_SevenSeg/DE1Soc_SevenSeg.h"
#include "DE1SoC_Servo/DE1SoC_Servo.h"
#include "stopWatchTimer.h"

// useful hardware address definitions
volatile unsigned int *key_ptr = (unsigned int *)0xFF200050; // KEYS 0-3 (push buttons)
volatile unsigned int *LEDR_ptr = (unsigned int *)0xFF200000; // LEDS 0-9
volatile unsigned int *key_edge_ptr = (unsigned int *)0xFF20005C;

#define RESET_STATE 0
#define STOP_STATE 1
#define START_STATE 2
#define SPLIT_STATE 3

#define CUMULATIVE_SPLIT 0
#define LAP_SPLIT 1

// Define new data type for a functions which takes an int and returns an int
//typedef unsigned int (*TaskFunction)(unsigned int);

//
// Function declarations
//

void configure_servoDrivers (void);

void stopWatch_updateStateMachine (TaskFunction*, unsigned int*, unsigned int*, unsigned int*, unsigned int*, unsigned int*, const unsigned int*,  unsigned int);

void stopWatch_updateSplitMode (unsigned int*);

void stopWatch_displayTime (unsigned int*);

void stopWatch_updateMetronome (unsigned int*);

void stopWatch_clearInputs (void);


#endif /* STOPWATCH_MAIN_H_ */
