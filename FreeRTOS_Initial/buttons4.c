// *******************************************************
// 
// buttons4:        Support for a set of FOUR specific buttons on the Tiva/Orbit.
//                  ENCE361 sample code.
//                  The buttons are:  UP and DOWN (on the Orbit daughterboard) plus
//                  LEFT and RIGHT on the Tiva.
//
//Note:             pin PF0 (the pin for the RIGHT pushbutton - SW2 on
//                  the Tiva board) needs special treatment - See PhilsNotesOnTiva.rtf.
//
// P.J. Bones UCECE
// Last modified:  7.2.2018
// 
// *******************************************************


// *******************************************************
// Standard Includes
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/debug.h"
#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)
#include "buttons4.h"


// *******************************************************
// OrbitOLED Includes
#include "OrbitOLED/OrbitOLEDInterface.h"
#include "OrbitOLED/lib_OrbitOled/OrbitOled.h"


// *******************************************************
//FreeRTOS Header Files
#include "priorities.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


// *******************************************************
// Globals to module
static bool but_state[NUM_BUTS];	// Corresponds to the electrical state
static uint8_t but_count[NUM_BUTS];
static bool but_flag[NUM_BUTS];
static bool but_normal[NUM_BUTS];   // Corresponds to the electrical state


// *******************************************************
// FreeRTOS Task Parameters
xQueueHandle g_pQueue;
extern xSemaphoreHandle g_pUARTSemaphore;

#define BUT_TASK_STACK_SIZE     128         // Stack size in words
#define BUT_ITEM_SIZE           sizeof(uint8_t)
#define BUT_QUEUE_SIZE          5



// *******************************************************
// initButtons:     Initialise the variables associated with the set of buttons
//                  defined by the constants in the buttons2.h header file.
// Author:          P.J. Bones UCECE
// Last modified:   7.2.2018
void
initButtons (void)
{
	int i;

	// UP button (active HIGH)
    SysCtlPeripheralEnable (UP_BUT_PERIPH);
    GPIOPinTypeGPIOInput (UP_BUT_PORT_BASE, UP_BUT_PIN);
    GPIOPadConfigSet (UP_BUT_PORT_BASE, UP_BUT_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);
    but_normal[UP] = UP_BUT_NORMAL;
	// DOWN button (active HIGH)
    SysCtlPeripheralEnable (DOWN_BUT_PERIPH);
    GPIOPinTypeGPIOInput (DOWN_BUT_PORT_BASE, DOWN_BUT_PIN);
    GPIOPadConfigSet (DOWN_BUT_PORT_BASE, DOWN_BUT_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);
    but_normal[DOWN] = DOWN_BUT_NORMAL;
    // LEFT button (active LOW)
    SysCtlPeripheralEnable (LEFT_BUT_PERIPH);
    GPIOPinTypeGPIOInput (LEFT_BUT_PORT_BASE, LEFT_BUT_PIN);
    GPIOPadConfigSet (LEFT_BUT_PORT_BASE, LEFT_BUT_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPU);
    but_normal[LEFT] = LEFT_BUT_NORMAL;
    // RIGHT button (active LOW)
      // Note that PF0 is one of a handful of GPIO pins that need to be
      // "unlocked" before they can be reconfigured.  This also requires
      //      #include "inc/tm4c123gh6pm.h"
    SysCtlPeripheralEnable (RIGHT_BUT_PERIPH);
    //---Unlock PF0 for the right button:
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R |= GPIO_PIN_0; //PF0 unlocked
    GPIO_PORTF_LOCK_R = GPIO_LOCK_M;
    GPIOPinTypeGPIOInput (RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN);
    GPIOPadConfigSet (RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPU);
    but_normal[RIGHT] = RIGHT_BUT_NORMAL;

	for (i = 0; i < NUM_BUTS; i++)
	{
		but_state[i] = but_normal[i];
		but_count[i] = 0;
		but_flag[i] = false;
	}
}

// *******************************************************
// updateButtons:   Function designed to be called regularly. It polls all
//                  buttons once and updates variables associated with the buttons if
//                  necessary.  It is efficient enough to be part of an ISR, e.g. from
//                  a SysTick interrupt.
//                  Debounce algorithm: A state machine is associated with each button.
//                  A state change occurs only after NUM_BUT_POLLS consecutive polls have
//                  read the pin in the opposite condition, before the state changes and
//                  a flag is set.  Set NUM_BUT_POLLS according to the polling rate.
// Author:          P.J. Bones UCECE
// Last modified:   7.2.2018
void
updateButtons (void)
{
	bool but_value[NUM_BUTS];
	int i;
	
	// Read the pins; true means HIGH, false means LOW
	but_value[UP] = (GPIOPinRead (UP_BUT_PORT_BASE, UP_BUT_PIN) == UP_BUT_PIN);
	but_value[DOWN] = (GPIOPinRead (DOWN_BUT_PORT_BASE, DOWN_BUT_PIN) == DOWN_BUT_PIN);
    but_value[LEFT] = (GPIOPinRead (LEFT_BUT_PORT_BASE, LEFT_BUT_PIN) == LEFT_BUT_PIN);
    but_value[RIGHT] = (GPIOPinRead (RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN) == RIGHT_BUT_PIN);
	// Iterate through the buttons, updating button variables as required
	for (i = 0; i < NUM_BUTS; i++)
	{
        if (but_value[i] != but_state[i])
        {
        	but_count[i]++;
        	if (but_count[i] >= NUM_BUT_POLLS)
        	{
        		but_state[i] = but_value[i];
        		but_flag[i] = true;	   // Reset by call to checkButton()
        		but_count[i] = 0;
        	}
        }
        else
        	but_count[i] = 0;
	}
}

// *******************************************************
// checkButton:     Function returns the new button logical state if the button
//                  logical state (PUSHED or RELEASED) has changed since the last call,
//                  otherwise returns NO_CHANGE.
// Author:          L. W. Trenberth
// Last modified:   5.8.2020
uint8_t
checkButton (uint8_t butName)
{
	if (but_flag[butName])
	{
		but_flag[butName] = false;
		if (but_state[butName] == but_normal[butName])
			return RELEASED;
		else
			return PUSHED;
	}
	return NO_CHANGE;
}

// *******************************************************
// ButTask:
// Author:          L. W. Trenberth
// Last modified:   5.8.2020
static void
ButTask(void *pvParameters)
{
    bool but_value[NUM_BUTS];
    int i;

    // Read the pins; true means HIGH, false means LOW
    but_value[UP] = (GPIOPinRead (UP_BUT_PORT_BASE, UP_BUT_PIN) == UP_BUT_PIN);
    but_value[DOWN] = (GPIOPinRead (DOWN_BUT_PORT_BASE, DOWN_BUT_PIN) == DOWN_BUT_PIN);
    but_value[LEFT] = (GPIOPinRead (LEFT_BUT_PORT_BASE, LEFT_BUT_PIN) == LEFT_BUT_PIN);
    but_value[RIGHT] = (GPIOPinRead (RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN) == RIGHT_BUT_PIN);


    // Iterate through the buttons, updating button variables as required
    for (i = 0; i < NUM_BUTS; i++)
    {
        if (but_value[i] != but_state[i])
        {
            but_count[i]++;
            if (but_count[i] >= NUM_BUT_POLLS)
            {
                but_state[i] = but_value[i];
                but_flag[i] = true;    // Reset by call to checkButton()
                but_count[i] = 0;
            }
        }
        else
            but_count[i] = 0;
    }
}

// *******************************************************
// initButTask:     Function initialises the FreeRTOS Button Task
// Author:          L. W. Trenberth
// Last modified:   5.8.2020
uint32_t
initButTask(void)
{
    //Create the task
    if(xTaskCreate(ButTask, (const portCHAR *)"BUT", BUT_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY + PRIORITY_BUT_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    return (0);
}
