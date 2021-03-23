# ELEC5620M Written Assignment Repository - Stopwatch
This repository contains the files required for the digital stopWatch.
The included files and project heirarchy are as follows

## Original IP 
### 1st level
| Filename 				| Descriptions                 			 |
|---|---|
| main.c     	| Program to handle the top level functionality of the stopwatch using a statemachine taking inputs from keys [2:0] and outputting to 7 segment LEDS, red LEDs, and servo 0. |
| main.h        | header file for main.c |

### 2nd level
| Filename 				| Descriptions                 			 |
|---|---|
| stopWatchTimer.c    	| A programme to handle the timer functionality of the stopwatch using a task scheduler and the private timer moduler.|
| stopWatchTimer.h        | header file for stopWatchTimer.c |

### 3rd level
| Filename | Description |
|---|---|
| HPS_PrivateTimer.c/.h | Functions for interacting with the private timer of the the DE1-SoC computer. |

## externally sourced driver files
# 2nd level
| Filename | Description | Company | Author/s |
|---|---|---|---|
| HPS_Watchdog.c./.h | Simple inline functions for resetting watchdog and returning the current watchdog timer value. | University of Leeds | T Carpenter |
| DE1SocSevenSeg.c/.h | Functions for displaying input values on the seven segment LEDs in the DE1-SoC computer | University of Leeds | Harry Clegg, Henry Fielding |
| DE1SoC_Servo.c/.h| Driver for the Servo Controller in the DE1-SoC Computer | University of Leeds | T Carpenter |


