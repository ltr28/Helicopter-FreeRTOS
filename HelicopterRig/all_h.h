/*
 * all_h_files_needed_for_the_project.h
 *
 *  Created on: 13/05/2019
 *  Team_members: Abhimanyu Chhabra(99799242), Saranya Ramani(27179399), Jin Kim(79903309)
 */

/*
  all_h.h file includes standard header files required for the project given by Texas Instruments, Digilent Inc,
  header files written by  P.J. Bones and mdp46.
*/

#ifndef ALL_H_H_
#define ALL_H_H_




// By:  Texas Instruments
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "stdlib.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"
#include "driverlib/adc.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "utils/ustdlib.h"


//By: Digilent Inc.
#include "../../OrbitOLED/lib_OrbitOled/OrbitOled.h"

// Author: P.J Bones
#include "buttons4.h"
#include "circBufT.h"

// Author: mdp46
#include "../../OrbitOLED/OrbitOLEDInterface.h"









#endif /* ALL_H_H_ */
