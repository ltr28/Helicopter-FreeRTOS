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

    peripherals_reset (); // all the peripherals are reset
    pheripherals_enable (); // all the peripherals are enabled
    init_clock();
    initialiseUSB_UART ();
    init_adc();
    g_pUARTSemaphore = xSemaphoreCreateMutex();



//    SysCtlDelay(SysCtlClockGet() / 12);
//    calculate_landed_position();




    if (initAltTask() != 0)
    {
        while(1)
        {
            //add blinking LED routine here
            //print to UART Altitude Task not working
        }
    }

    if(Altitude_calc() != 0 )
    {
       while(1)
       {
              printf("Virus\n");
       }
    }


    vTaskStartScheduler();

    while(1)
    {
    }
}






































//    if(initYawTask() != 0){
//        while(1)
//        {
//            //add blinking LED routine here
//            //print to UART Yaw Task not working
//        }
//    }



//
////    if(initControlTask != 0){
////        while(1)
////        {
////            //add blinking LED routine here
////            //print to UART Control Task not working
////        }
////
////    }
////
//    if(initButTask() != 0){
//        while(1)
//        {
//            //add blinking LED routine here
//            //print to UART Button Task not working
//        }
//
//    }
////
////    if(initPWMTask() != 0){
////          while(1)
////          {
////              //add blinking LED routine here
////              //print to UART PWM Task not working
////          }
////
////      }
////
//    if(initDisplayTask() != 0) {
//          while(1)
//          {
//              //add blinking LED routine here
//              //print to UART PWM Task not working
//          }
//
//      }


