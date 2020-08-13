//*****************************************************************************
//
// Milestone 1 - Displaying helicopter altitude as a percentage
//
// Author:  N. James
//          L. Trenberth
//          A. Chhabra
// Last modified:	31.5.2019

#include <AllHeaderFiles.h>
#include "altitude.h"
#include "uart.h"
#include "yaw.h"
#include "buttons4.h"
#include "control.h"

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
    peripherals_reset (); // all the peripherals are reset
    pheripherals_enable (); // all the peripherals are enabled

    init_clock();
    init_pwm_clock ();
    initButtons();
    initialiseUSB_UART();
    init_adc();
    initYaw();
    init_button_timer();
    init_pwm ();
    init_slider_switch_and_yaw_reference_pins();
    turn_on_pwm_output();
    set_initial_value_of_slider_switch();
    IntMasterEnable();


    if (initAltTask() != 0)
    {
        while(1)
        {

        }
    }

    if(Altitude_calc() != 0 )
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

    if(initButTask() != 0){
        while(1)
        {

        }

    }

    if(initControlTask != 0){
        while(1)
        {

        }

    }



    vTaskStartScheduler();

    while(1)
    {

    }

}












































////

////

////
//    if(initDisplayTask() != 0) {
//          while(1)
//          {
//              //add blinking LED routine here
//              //print to UART PWM Task not working
//          }
//
//      }


