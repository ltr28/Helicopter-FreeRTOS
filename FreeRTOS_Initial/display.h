#ifndef DISPLAY_H_
#define DISPLAY_H_

//*****************************************************************************
//
// Display - Display related functions
//
// Author:  N. James
//          L. Trenberth
//          M. Arunchayanon
// Last modified:   20.4.2019
//*****************************************************************************


#include <stdint.h>

#define DISPTASKSTACKSIZE        128         // Stack size in words
#define DISP_ITEM_SIZE           sizeof(uint32_t)
#define DISP_QUEUE_SIZE          5
#define MAX_STR_LEN              100

//  *****************************************************************************
//  initDisplay:        Initialises Display using OrbitLED functions
void
initDisplay (void);


//  *****************************************************************************
//  introLine:          Prints the intro line on the OLED Display
void
introLine (void);


//  *****************************************************************************
//  printString:        Prints the input format and line contents on the given line number on OLED Display
//  TAKES:              line_format - The format to print the string in, including a integer placeholder
//                      line_contents - The integer to print on the line
//                      line_number - The line number integer to print the string on.
void
printString(char* restrict line_format, int32_t line_contents, uint8_t line_number);


//*****************************************************************************
//  initButtonCheck:    Initialises left and up buttons on the micro-controller
void
initButtonCheck (void);


//  *****************************************************************************
//  OutputToDisplay:    Displays the helicopter altitude, height and references
//  NOTE:               This function is not currently implemented, though is included for testing
void
OutputToDisplay (void);


//*****************************************************************************
//  OutputToDisplay:    Uses UART to send yaw and altitude references,
//                      duty cycles and the current mode
void
OutputToUART (void);


#endif /*DISPLAY_H_*/
