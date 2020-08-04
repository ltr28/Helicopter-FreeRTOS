//*****************************************************************************
//
// Milestone 1 - Displaying helicopter altitude as a percentage
//
// Author:  N. James
//          L. Trenberth
//          A. Chhabra
// Last modified:	31.5.2019

//*****************************************************************************
//FreeRTOS Includes
#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "utils/uartstdio.h"
//*****************************************************************************

//
//*****************************************************************************
// Included Files
//*****************************************************************************
#include "system.h"
#include "altitude.h"
#include "display.h"
#include "yaw.h"
#include "motor.h"
#include "control.h"
#include "uart.h"

#define mainDELAY_LOOP_COUNT 4000

//*****************************************************************************
//
// The mutex that protects concurrent access of UART from multiple tasks.
//
//*****************************************************************************
xSemaphoreHandle g_pUARTSemaphore;


//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//  initAll: Initialises all buttons, interrupts, ADC, PWM, modes and controls
void initAll (void) {
    initClock();

    resetmotor();
    initButtonCheck();
    initADC();
    initYaw();
    initDisplay();
  //initSysTick();
     initButtons();
    initSwitch_PC4();
    //initDisplay();
    initialiseUSB_UART();
    initCircBuf(bufferLocation(), BUF_SIZE);
    initmotor();
    SysCtlDelay(SysCtlClockGet() / 12);
    resetAltitude();
}

//*****************************************************************************
// Control:            Controls the altitude and yaw of a model helicopter
void control (void)
{
    helicopterStates();
    OutputToUART();
    GetSwitchState();
    PIDControlAlt();
    PIDControlYaw();
}

void altTask (void *pvparameters )
{
    volatile uint32_t ul;
    //cost char *pcTaskName = "Running the Height Task\r\n";
    for( ;; )
    {
        //OLEDStringDraw("Display Altitude", 0, 0);
        //writeCircBuf()
        vTaskDelay(10);
    }
}

//

void dispTask (void *pvparameters, int yaw)
{
    //if the length of the queue for writing to the display is greater than 0 write to the screen
    printString("Yaw = %4d%%", readCircBuf(bufferLocation()), 0);
    freeCircBuf(bufferLocation());

}



//*****************************************************************************
// Main:            Controls the altitude and yaw of a model helicopter
int main(void)
{
    initAll();
    xTaskCreate( yawTask, "Yaw Task", 1000, NULL, 2, NULL); {
        while(1){
            //add blinking LED routine in here
        }
    }

    xTaskCreate( dispTask, "Display Task", 1000, NULL, 1, NULL);
    {
        while(1) {
            //add blinking LED routine in here
        }
    }
    vTaskStartScheduler();
    while(1)
    {
    }
}

