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
#include <heliQueue.h>
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
    initButtons();
    initSwitch_PC4();
    initialiseUSB_UART();
    initCircBuf(bufferLocation(), BUF_SIZE);
    initmotor();
    SysCtlDelay(SysCtlClockGet() / 12);
    resetAltitude();
}

//*****************************************************************************
// Main:            Controls the altitude and yaw of a model helicopter
int main(void)
{
    initAll();


//    if(initYawTask() != 0){
//        while(1)
//        {
//            //add blinking LED routine here
//            //print to UART Yaw Task not working
//        }
//    }

    if (initAltTask() != 0){
        while(1)
        {
            //add blinking LED routine here
            //print to UART Altitude Task not working
        }
    }

//    if(initControlTask != 0){
//        while(1)
//        {
//            //add blinking LED routine here
//            //print to UART Control Task not working
//        }
//
//    }
//
//    if(initButtonsTask() != 0){
//        while(1)
//        {
//            //add blinking LED routine here
//            //print to UART Button Task not working
//        }
//
//    }
//
//    if(initPWMTask() != 0){
//          while(1)
//          {
//              //add blinking LED routine here
//              //print to UART PWM Task not working
//          }
//
//      }
//
//    if(initDisplayTask() != 0){
//          while(1)
//          {
//              //add blinking LED routine here
//              //print to UART PWM Task not working
//          }
//
//      }




    vTaskStartScheduler();

    while(1)
    {
    }
}

