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
#include "HPS_Watchdog/HPS_Watchdog.h"
#include "HPS_PrivateTimer/HPS_PrivateTimer.h"
#include "DE1Soc_SevenSeg/DE1Soc_SevenSeg.h"
#include "DE1SoC_Servo/DE1SoC_Servo.h"
#include "DE1SoC_LT24/DE1SoC_LT24.h"

// useful hardware address definitions
volatile unsigned int *key_ptr = (unsigned int *)0xFF200050; // KEYS 0-3 (push buttons)
volatile unsigned int *LEDR_ptr = (unsigned int *)0xFF200000; // LEDS 0-9
volatile unsigned int *key_edge_ptr = (unsigned int *)0xFF20005C;

// Define new data type for a functions which takes an int and returns an int
typedef unsigned int (*TaskFunction)(unsigned int, unsigned int);

//
// Function declarations
//

void configure_privateTimer (void);


void configure_servoDrivers (void);


unsigned int update_hundredths (unsigned int hours, unsigned int hundredths);


unsigned int update_seconds (unsigned int hours, unsigned int seconds);


unsigned int update_minutes (unsigned int hours, unsigned int minutes);


unsigned int update_hours (unsigned int hours, unsigned int unused);


void reset_stopWatch (unsigned int* time, unsigned int* taskLastTime, unsigned int taskCount);



#endif /* STOPWATCH_MAIN_H_ */
