/*
    uart.h (Header file for uart.c)

    Created on: 16/05/2019
    Team_members: Abhimanyu Chhabra(99799242), Saranya Ramani(27179399), Jin Kim(79903309)
 */


#ifndef UART_H_
#define UART_H_


// Constants
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


// Functions declarations

// initialiseUSB_UART - 8 bits, 1 stop bit, no parity
void
initialiseUSB_UART (void);

// display data over UART
void
uart_display(void);
#endif /* UART_H_ */
