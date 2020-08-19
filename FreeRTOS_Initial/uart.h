/*
 * uart.h
 *
 *  Created on: May 20, 2019
 *      Author: par116
 */

#ifndef UART_H_
#define UART_H_




#include <AllHeaderFiles.h>




//---USB Serial comms: UART0, Rx:PA0 , Tx:PA1
#define BAUD_RATE 9600
#define UART_USB_BASE           UART0_BASE
#define UART_USB_PERIPH_UART    SYSCTL_PERIPH_UART0
#define UART_USB_PERIPH_GPIO    SYSCTL_PERIPH_GPIOA
#define UART_USB_GPIO_BASE      GPIO_PORTA_BASE
#define UART_USB_GPIO_PIN_RX    GPIO_PIN_0
#define UART_USB_GPIO_PIN_TX    GPIO_PIN_1
#define UART_USB_GPIO_PINS      UART_USB_GPIO_PIN_RX | UART_USB_GPIO_PIN_TX

//********************************************************
// Prototypes
//********************************************************
void
initialiseUSB_UART (void);


void
UARTSend (char *pucBuffer);

void
yawTask (void *pvparameters);

int32_t
initUARTTask(void);

void
UARTTask(void *pvparameters);

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


static void
DisplayTask(void *pvparameters);


//  *****************************************************************************
//  initDisplayTask:    Initialises the FreeRTOS Task displaying the helicopter altitude, height and references
extern int32_t
initDisplayTask(void);

#endif /* UART_H_ */
