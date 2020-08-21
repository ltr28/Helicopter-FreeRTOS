/*****************************************************************************

 ENCE 464 Heli Rig - uart.c

 Author:            Nathan James (44005459)
                    Luke Trenberth (47277086)
                    Abhimanyu Chhabra (99799242)

 Last modified:     21.08.2020

 Purpose:           Contains functions to initialise and control UART on the Helicopter Rigs

 */

#include "uart.h"
#include "AllHeaderFiles.h"
#include "control.h"

xSemaphoreHandle g_pUARTSemaphore;
xSemaphoreHandle g_pDataSemaphore;
OperatingData_t OperatingData;



//  *****************************************************************************
//  initialiseUSB_UART:     Initializes UART with 8 bits, 1 stop bit, no parity
void
initialiseUSB_UART (void)
{

    // Enable GPIO port A which is used for UART0 pins.
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);

    // Select the alternate (UART) function for these pins.
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



//  *****************************************************************************
//  UARTSend:               Transmit a string via UART0
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

//  *****************************************************************************
//  UARTTask:               FreeRTOS UART Task
//                          First set of comments for Emulator, Seconds for Rig
void
UARTTask(void *pvparameters)
{
    TickType_t xTime;
    xTime = xTaskGetTickCount();
    while(1)

    {
        //Emulator UART
//        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
//        xSemaphoreTake(g_pDataSemaphore, portMAX_DELAY);
//        //UARTprintf("\033[H\033[2JENCE461 HeliRig Emulator\n");
//        UARTprintf("\nMode: %d\n", (int) OperatingData.HeliMode);
//        UARTprintf("\nProperties:\n");
//        UARTprintf("ALT: %d\n", (int) OperatingData.AltCurrent);
//        UARTprintf("YAW: %d\n", (int)OperatingData.YawCurrentMapped);
//        UARTprintf("ALT REF: %d\n", (int) OperatingData.AltRef);
//        UARTprintf("YAW REF: %d\n", (int) OperatingData.YawRefMapped);
//        UARTprintf("\nMotor Properties:\n");
//        UARTprintf("ALT DUTY: %d\n", OperatingData.AltDuty);
//        UARTprintf("YAW DUTY: %d\n", OperatingData.YawDuty);
//
//        xSemaphoreGive(g_pDataSemaphore);
//        xSemaphoreGive(g_pUARTSemaphore);
//        vTaskDelayUntil(&xTime, pdMS_TO_TICKS(100));

        //Lab Share UART
        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
        xSemaphoreTake(g_pDataSemaphore, portMAX_DELAY);
        UARTprintf("ALT: %d", (int) OperatingData.AltCurrent);
        UARTprintf("YAW: %d", (int)OperatingData.YawCurrentMapped);
        UARTprintf("ALT REF: %d", (int) OperatingData.AltRef);
        UARTprintf("YAW REF: %d", (int) OperatingData.YawRefMapped);
        UARTprintf("ALT DUTY: %d", OperatingData.AltDuty);
        UARTprintf("YAW DUTY: %d", OperatingData.YawDuty);

        xSemaphoreGive(g_pDataSemaphore);
        xSemaphoreGive(g_pUARTSemaphore);
        vTaskDelayUntil(&xTime, pdMS_TO_TICKS(100));
    }
}

//  *****************************************************************************
//  initUARTTask:           Initialises the FreeRTOS UART Task
int32_t initUARTTask(void)
{
    if(xTaskCreate(UARTTask, (const portCHAR *)"UART", UART_TASK_STACK_SIZE, NULL,
                       PRIORITY_UART_TASK, NULL) != pdTRUE)
        {
            return(1); //Error occurred
        }
        //
        // Success.
        //
        return(0);
}

OperatingData_t OperatingData;
volatile uint8_t slowTick = false;

//  *****************************************************************************
//  initDisplay:            Initialises Display using OrbitLED functions
void initDisplay (void)
{
    // Initialise the Orbit OLED display
    OLEDInitialise ();
}


//*****************************************************************************
//  OutputToDisplay:        Prints data on the OLED Display. Used on the microcontroller
void OutputToDisplay(void)
{
    char buffer[16];
    sprintf (buffer, "ALT: %d", OperatingData.AltCurrent);
    OLEDStringDraw (buffer, 0, 0); // Update line on display.
    sprintf (buffer, "YAW: %d", OperatingData.YawCurrentMapped);
    OLEDStringDraw (buffer, 0, 1); // Update line on display.
    sprintf (buffer, "ALT REF: %d", OperatingData.AltRef);
    OLEDStringDraw (buffer, 0, 2); // Update line on display.
    sprintf (buffer, "YAW REF: %d", OperatingData.YawRefMapped);
    OLEDStringDraw (buffer, 0, 3); // Update line on display.
}



//  *****************************************************************************
//  DisplayTask:            FreeRTOS Task displaying the helicopter altitude, height and references on the OLED Display
static void DisplayTask(void *pvparameters)
{
    TickType_t xTime;
    xTime = xTaskGetTickCount();
    while(1)
    {
        OutputToDisplay();
        vTaskDelayUntil(&xTime, pdMS_TO_TICKS(100));
    }
}

//  *****************************************************************************
//  initDisplayTask:        Initialises the FreeRTOS Task displaying the helicopter altitude, height and references
int32_t initDisplayTask(void)
{
    if(xTaskCreate(DisplayTask, (const portCHAR *)"DISPLAY", DISPLAY_TASK_STACK_SIZE,
                   NULL, PRIORITY_DISPLAY_TASK, NULL) != pdTRUE)
     {
         return(1);
     }

     return (0);
}
