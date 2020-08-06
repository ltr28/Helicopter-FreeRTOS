/*
 * uart.h
 *
 *  Created on: May 20, 2019
 *      Author: par116
 */

#ifndef UART_H_
#define UART_H_


#include "allheaderfiles.h"



#define MAX_STR_LEN 300

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


#endif /* UART_H_ */
