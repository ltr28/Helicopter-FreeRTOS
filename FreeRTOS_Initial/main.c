//*****************************************************************************
//
// ENCE 464 Heli Rig - Main Function
//
// Author:  Nathan James ()
//          Luke Trenberth (47277086)
//          Abhimanyu Chhabra (99799242)
// Last modified:	20.08.2020

#include <AllHeaderFiles.h>

#include "altitude.h"
#include "uart.h"
#include "yaw.h"
#include "buttons4.h"
#include "control.h"
#include "pwm.h"


//*****************************************************************************
// The mutex that protects concurrent access of UART from multiple tasks.
xSemaphoreHandle g_pUARTSemaphore;
xSemaphoreHandle g_pDataSemaphore;
xSemaphoreHandle g_pADCSemaphore;

OperatingData_t OperatingData;

//*****************************************************************************
// This hook is called by FreeRTOS when an stack overflow error is detected.
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
init_clock (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                    SYSCTL_XTAL_16MHZ);
}

void peripherals_reset(void)
{
    SysCtlPeripheralReset (SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralReset (SYSCTL_PERIPH_UART0);
    SysCtlPeripheralReset (SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralReset (SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralReset (SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralReset (SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralReset (SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralReset (SYSCTL_PERIPH_GPIOF);

}

void pheripherals_enable(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}



int main(void)
{
    g_pUARTSemaphore = xSemaphoreCreateMutex();
    g_pDataSemaphore = xSemaphoreCreateMutex();
    OperatingData = OperatingData_init();

    peripherals_reset (); // all the peripherals are reset
    pheripherals_enable (); // all the peripherals are enabled
    init_clock();
    initPWMClock();
    initButtons();
    initSwitches();
    initialiseUSB_UART();
    initADC();
    initYaw();
    initButtonTimer();
    initSliderSwitch();
    initPWM ();
    PWMOn();
    initDisplay();
    IntMasterEnable();

    if (initADCTriggerTask() != 0)
    {
        while(1)
        {

        }
    }

    if(intADCReceiveTask() != 0 )
    {
        while(1)
        {

        }
    }

    if(inityawTask() != 0){
        while(1)
        {

        }
    }

    if(initButtonTask() != 0){
        while(1)
        {

        }

    }

    if(initControlTask() != 0){
        while(1)
        {

        }

    }

    if(initUARTTask() != 0) {
          while(1)
          {

          }

      }

    if(initDisplayTask() != 0) {
          while(1)
          {

          }

      }




    vTaskStartScheduler();

    while(1)
    {

    }

}




