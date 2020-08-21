/*****************************************************************************

 ENCE 464 Heli Rig - uart.h

 Author:            Nathan James (44005459)
                    Luke Trenberth (47277086)
                    Abhimanyu Chhabra (99799242)

 Last modified:     21.08.2020

 Purpose:           Contains functions to initialise and control UART on the Helicopter Rigs

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


//  *****************************************************************************
//  initialiseUSB_UART:     Initializes UART with 8 bits, 1 stop bit, no parity
void
initialiseUSB_UART (void);



//  *****************************************************************************
//  UARTSend:               Transmit a string via UART0
void
UARTSend (char *pucBuffer);



//  *****************************************************************************
//  UARTTask:               FreeRTOS UART Task
//                          First set of comments for Emulator, Seconds for Rig
void
UARTTask(void *pvparameters);



//  *****************************************************************************
//  initUARTTask:           Initialises the FreeRTOS UART Task
int32_t
initUARTTask(void);



//  *****************************************************************************
//  initDisplay:        Initialises Display using OrbitLED functions
void
initDisplay (void);




//  *****************************************************************************
//  OutputToDisplay:    Displays the helicopter altitude, height and references
//  NOTE:               This function is not currently implemented, though is included for testing
void
OutputToDisplay (void);



//  *****************************************************************************
//  DisplayTask:            FreeRTOS Task displaying the helicopter altitude, height and references on the OLED Display
static void
DisplayTask(void *pvparameters);



//  *****************************************************************************
//  initDisplayTask:    Initialises the FreeRTOS Task displaying the helicopter altitude, height and references
extern int32_t
initDisplayTask(void);

#endif /* UART_H_ */
