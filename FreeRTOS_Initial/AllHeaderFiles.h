/*****************************************************************************

 ENCE 464 Heli Rig - AllHeaderFiles.c

 Author:            Nathan James (44005459)
                    Luke Trenberth (47277086)
                    Abhimanyu Chhabra (99799242)

 Last modified:     21.08.2020

Purpose:            Establishes a header file that most files require
                    Imported by all .c files
 */

#ifndef ALLHEADERFILES_H_
#define ALLHEADERFILES_H_

// By:  Texas Instruments
#include "stdint.h"
#include "stdbool.h"
#include "stdio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)


#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/pwm.h"
#include "driverlib/adc.h"
#include "driverlib/interrupt.h"
#include "driverlib/debug.h"
#include "driverlib/rom.h"
#include "driverlib/debug.h"


#include "utils/ustdlib.h"
#include "utils/uartstdio.h"


//FreeRTOS Includes
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "priorities.h"

//OrbitOLED Includes
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OrbitOLED/lib_OrbitOled/OrbitOled.h"


//Object Oriented Includes
#include "data_control.h"
#include "pid.h"

OperatingData_t OperatingData;

#endif /* ALLHEADERFILES_H_ */
