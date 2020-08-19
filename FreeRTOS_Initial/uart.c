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
#include <altitude.h>
#include "yaw.h"
#include "pid.h"
#include "control.h"

extern PID_t Alt_PID;
extern PID_t Yaw_PID;

extern xSemaphoreHandle g_pUARTSemaphore;
extern OperatingData_t OperatingData;

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
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        UARTprintf("\033[H\033[2JENCE461 HeliRig Emulator\n");
        UARTprintf("\nMode: %d\n", (int) OperatingData.HeliMode);
        UARTprintf("\nProperties:\n");
        UARTprintf("ALT: %d\n", (int) OperatingData.AltCurrent);
        UARTprintf("YAW: %d\n", (int)OperatingData.YawCurrentMapped);
        UARTprintf("ALT REF: %d\n", (int) OperatingData.AltRef);
        UARTprintf("YAW REF: %d\n", (int) OperatingData.YawRefMapped);
        UARTprintf("\nMotor Properties:\n");
        UARTprintf("ALT DUTY: %d\n", OperatingData.AltDuty);
        UARTprintf("YAW DUTY: %d\n", OperatingData.YawDuty);


        xSemaphoreGive(g_pUARTSemaphore);
        vTaskDelayUntil(&xTime, pdMS_TO_TICKS(100));
    }
}

int32_t initUARTTask(void)
{
    if(xTaskCreate(UARTTask, (const portCHAR *)"UART", 100, NULL,
                       PRIORITY_UART_TASK, NULL) != pdTRUE)
        {
            return(1); //Error occurred
        }
        //
        // Success.
        //
        return(0);
}

