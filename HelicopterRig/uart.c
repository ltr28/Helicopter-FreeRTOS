/*
    uart.c

    Created on: 16/05/2019
    Team_members: Abhimanyu Chhabra(99799242), Saranya Ramani(27179399), Jin Kim(79903309)
 */

/*
   Simple initialization of UART.
   Functions based on week4_lab uartDemo.c file -  (Author:  P.J. Bones)
   1. void initialiseUSB_UART (void)
   2. void UARTSend (char *pucBuffer)
 */

#include "all_h.h"
#include "uart.h"
#include "pid_control.h"
#include "adc.h"
#include "yaw.h"



/*
   initialiseUSB_UART - 8 bits, 1 stop bit, no parity
 */
void
initialiseUSB_UART (void)
{

    // Enable GPIO port A which is used for UART0 pins.


    // Select the alternate (UART) function for these pins.

    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);

    UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);



    UARTFIFOEnable(UART_USB_BASE);

    UARTEnable(UART_USB_BASE);
}


/*
   Transmit a string via UART0
 */
void
UARTSend (char *pucBuffer)
{
    // Loop while there are more characters to send.
    while(*pucBuffer)
    {
        // Write the next character to the UART Tx FIFO.
        UARTCharPut(UART_USB_BASE, *pucBuffer);
        pucBuffer++;
    }
}

/*
   void uart_display(void) sends the following data over UART at 4Hz: (Function called in while(1) in Heli_Main.c)
   1.current altitude - get_percentage()
   2.desired altitude - get_set_alt_point()
   3.current degrees(within 360 to -360) - get_mapped_degress()
   4.desired yaw position  - get_mapped_set_yaw_point()(within 360 to -360)
   5.tail motor duty cycle - get_yaw_duty()
   6.main motor duty cycle - get_alt_duty()
   7.current flight mode - get_current_flight_mode()
 */
void
uart_display(void)
{

    char status_str[MAX_STR_LEN + 1]; // string size is MAX_STR_LEN + 1 = 300 + 1 = 301
    usprintf (status_str,            // put everything in the status_str
              "Alt= %d [%d]\r\n"
              "Yaw= %d [%d]\r\n"
              "tail_pwm = %d\r\n"
              "Main_pwm = %d\r\n"
              "Mode = %s\r\n",
              get_percentage(),get_set_alt_point(),
              get_mapped_degress(),get_mapped_set_yaw_point(),
              get_yaw_duty(),
              get_alt_duty(),
              get_current_flight_mode());
    UARTSend(status_str); // sending the status string(status_str) to UART
}



