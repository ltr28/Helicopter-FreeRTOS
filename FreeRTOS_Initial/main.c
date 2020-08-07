//*****************************************************************************
//
// Milestone 1 - Displaying helicopter altitude as a percentage
//
// Author:  N. James
//          L. Trenberth
//          A. Chhabra
// Last modified:   31.5.2019

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
#include "semphr.h"
#include "queue.h"
#include "utils/uartstdio.h"
//*****************************************************************************

//
//*****************************************************************************
// Included Files
//*****************************************************************************
#include "altitude.h"
#include "display.h"
#include "yaw.h"
#include "motor.h"
#include "control.h"
#include "driverlib/uart.h"
#include "buttons4.h"
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


void
initClock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                    SYSCTL_XTAL_16MHZ);
    //
    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
   // SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);
    //
    // Register the interrupt handler
   // SysTickIntRegister(SysTickIntHandler);
    //
    // Enable interrupt and device
    //SysTickIntEnable();
    //SysTickEnable();
}


//*****************************************************************************
//  initAll: Initialises all buttons, interrupts, ADC, PWM, modes and controls
void initAll (void) {
    //resetmotor();
    //initButtonCheck();
    initClock();
    initialiseUSB_UART();
    init_ADC();
    //initYaw();
    //initDisplay();
    //initButtons();
    //initSwitch_PC4();
    //initmotor();
   //
    SysCtlDelay(SysCtlClockGet() / 12);
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

    g_pUARTSemaphore = xSemaphoreCreateMutex();

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
//    if(initButTask() != 0){
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
