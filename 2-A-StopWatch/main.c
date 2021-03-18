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



main() {


	// local variables
	unsigned int hundredths;
	unsigned int lastTime;
	unsigned int taskInterval = 2250000;



	// configure ARM private timer
	Timer_initialise(0xFFFEC600);
	Timer_setLoadValue(0xFFFFFFFF);
	Timer_setControl(0, 0, 1, 1);

	while(1) {
		// Read the current time
		unsigned int currentTimerValue = Timer_readTimer();

		if ((lastTime - currentTimerValue) >= taskInterval) {
			if (hundredths > 99) hundredths = 1;
			else hundredths = hundredths + 1;
			DE1SoC_SevenSeg_SetDoubleDec(0, hundredths);


			lastTime = lastTime - taskInterval;
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
