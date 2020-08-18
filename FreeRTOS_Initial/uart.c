//********************************************************
//
// uartDemo.c - Example code for ENCE361
//
// Link with modules:  buttons2, OrbitOLEDInterface
//
// Author:  P.J. Bones  UCECE
// Last modified:   16.4.2018
//

#include <AllHeaderFiles.h>
#include <uart.h>
#include "altitude.h"
#include "yaw.h"
#include "control.h"


//********************************************************
// initialiseUSB_UART - 8 bits, 1 stop bit, no parity
//********************************************************
void
initialiseUSB_UART (void)
{
    //
    // Enable GPIO port A which is used for UART0 pins.
    //
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);
    //
    // Select the alternate (UART) function for these pins.
    //
    GPIOPinTypeUART(UART_USB_GPIO_BASE, UART_USB_GPIO_PINS);
    GPIOPinConfigure (GPIO_PA0_U0RX);
    GPIOPinConfigure (GPIO_PA1_U0TX);

   UARTConfigSetExpClk(UART_USB_BASE, SysCtlClockGet(), BAUD_RATE,
            UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
            UART_CONFIG_PAR_NONE);

    UARTStdioConfig(0, 9600, 20000000 );
    UARTFIFOEnable(UART_USB_BASE);
    UARTEnable(UART_USB_BASE);

}


//**********************************************************************
// Transmit a string via UART0
//**********************************************************************
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

void
UARTTask(void *pvparameters)
{
    TickType_t xTime;
    xTime = xTaskGetTickCount();
    while(1)

    {
        UARTprintf("ALT: %d\n", get_percentage());
        UARTprintf("YAW: %d\n", get_actual_degrees());
        UARTprintf("ALT REF: %d\n", get_alt_ref());
        UARTprintf("YAW REF: %d\n\n\n", get_yaw_ref());
        vTaskDelayUntil(&xTime, pdMS_TO_TICKS(10));
    }
}

int32_t initUARTTask(void)
{
    if(xTaskCreate(UARTTask, (const portCHAR *)"UART", 200, NULL,
                       PRIORITY_YAW_TASK, NULL) != pdTRUE)
        {
            return(1); //Error occurred
        }
        //
        // Success.
        //
        return(0);
}

