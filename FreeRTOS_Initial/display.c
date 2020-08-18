//*****************************************************************************
//
//  Display - Functions used to output values on Display and UART
//
//  Author:  N. James
//           L. Trenberth
//           A. Chhabbra
//  Last modified:   15.08.2020
//*****************************************************************************


#include "display.h"
#include "AllHeaderFiles.h"
#include "buttons4.h"
#include "altitude.h"
#include "yaw.h"
#include "control.h"


volatile uint8_t slowTick = false;

//  *****************************************************************************
//  initDisplay:        Initialises Display using OrbitLED functions
void initDisplay (void)
{
    // Initialise the Orbit OLED display
    OLEDInitialise ();
}


//  *****************************************************************************
//  printString:        Prints the input format and line contents on the given line number on OLED Display
//  Parameters:         line_format - The format to print the string in, including a integer placeholder
//                      contents - The integer to print on the line
//                      line_number - The line number integer to print the string on.
void printString(char* restrict line_format, int32_t contents, uint8_t line_number)
{
    char string[MAX_STR_LEN + 1];
    usnprintf (string, sizeof(string), line_format, contents);
    OLEDStringDraw (string, 0, line_number); // Update line on display.
}


//*****************************************************************************
//  OutputToDisplay:       Prints data on the OLED Display. Used on the microcontroller
void OutputToDisplay(void)
{
    printString("ALT: %d", get_percentage(), 0);
    printString("YAW: %d", get_actual_degrees(), 1);
    printString("ALT REF: %d", get_alt_ref(), 2);
    printString("YAW REF: %d", get_yaw_ref(), 3);
}


//  *****************************************************************************
//  DisplayTask:    FreeRTOS Task displaying the helicopter altitude, height and references on the OLED Display
static void DisplayTask(void *pvparameters)
{
    TickType_t xTime;
    xTime = xTaskGetTickCount();
    printString("Point %d", 1, 0);
    while(1)
    {
        OutputToDisplay();
        vTaskDelayUntil(&xTime, pdMS_TO_TICKS(100));
    }
}

//  *****************************************************************************
//  initDisplayTask:    Initialises the FreeRTOS Task displaying the helicopter altitude, height and references
int32_t initDisplayTask(void)
{
    if(xTaskCreate(DisplayTask, (const portCHAR *)"DISPLAY", 100, NULL, PRIORITY_DISP_TASK, NULL) != pdTRUE)
     {
         return(1);
     }

     return (0);
}

