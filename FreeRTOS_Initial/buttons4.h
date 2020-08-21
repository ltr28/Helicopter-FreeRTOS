/*****************************************************************************

 ENCE 464 Heli Rig - buttons4.c

 Author:            Nathan James (44005459)
                    Luke Trenberth (47277086)
                    Abhimanyu Chhabra (99799242)

 Last modified:     21.08.2020

 Purpose:           Support for a set of FOUR specific buttons on the Tiva/Orbit.

                    The ADC is being used to calculate the altitude of the helicopter. ADC samples at 100 Hz (0.01s) and then
                    writes it to a circular buffer. ADCProcessorTrigger(ADC0_BASE, 3) gets the sample - it is being called in Heli_Main.c
                    in void SysTickIntHandler(void).

                    Functions used from week 4 lab ADCdemo1.c (Author:  P.J. Bones) are:
                    1. void ADCIntHandler(void) -  void adc_int_handler(void)
                    2. void initADC (void)      -  void init_adc (void)
 */

//Note:             pin PF0 (the pin for the RIGHT pushbutton - SW2 on
//                  the Tiva board) needs special treatment - See PhilsNotesOnTiva.rtf.

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "AllHeaderFiles.h"
#include "timers.h"


//*****************************************************************************
// Constants
//*****************************************************************************
enum butNames {UP = 0, DOWN, LEFT, RIGHT, NUM_BUTS};
enum butStates {RELEASED = 0, PUSHED, NO_CHANGE};
// UP button
#define UP_BUT_PERIPH  SYSCTL_PERIPH_GPIOE
#define UP_BUT_PORT_BASE  GPIO_PORTE_BASE
#define UP_BUT_PIN  GPIO_PIN_0
#define UP_BUT_NORMAL  false
// DOWN button
#define DOWN_BUT_PERIPH  SYSCTL_PERIPH_GPIOD
#define DOWN_BUT_PORT_BASE  GPIO_PORTD_BASE
#define DOWN_BUT_PIN  GPIO_PIN_2
#define DOWN_BUT_NORMAL  false
// LEFT button
#define LEFT_BUT_PERIPH  SYSCTL_PERIPH_GPIOF
#define LEFT_BUT_PORT_BASE  GPIO_PORTF_BASE
#define LEFT_BUT_PIN  GPIO_PIN_4
#define LEFT_BUT_NORMAL  true
// RIGHT button
#define RIGHT_BUT_PERIPH  SYSCTL_PERIPH_GPIOF
#define RIGHT_BUT_PORT_BASE  GPIO_PORTF_BASE
#define RIGHT_BUT_PIN  GPIO_PIN_0
#define RIGHT_BUT_NORMAL  true

#define NUM_BUT_POLLS 3

#define SLIDER_SWITCH_GPIO_BASE GPIO_PORTA_BASE
#define SLIDER_SWITCH_GPIO_PIN GPIO_PIN_7
#define YAW_REFERENCE_GPIO_BASE GPIO_PORTC_BASE
#define YAW_REFERENCE_GPIO_PIN GPIO_PIN_4


// Debounce algorithm:  A state machine is associated with each button.
//                      A state change occurs only after NUM_BUT_POLLS consecutive polls have
//                      read the pin in the opposite condition, before the state changes and
//                      a flag is set.  Set NUM_BUT_POLLS according to the polling rate.


// *******************************************************
// initButtons:         Initialise the variables associated with the set of buttons
//                      defined by the constants above.
void
initButtons (void);

void
initSwitches(void);
//// *******************************************************
//// updateButtons:       Function designed to be called regularly. It polls all
////                      buttons once and updates variables associated with the buttons if
////                      necessary.  It is efficient enough to be part of an ISR, e.g. from
////                      a SysTick interrupt.
//void
//updateButtons (void);
//
//
//// *******************************************************
//// checkButton:         Function returns the new button state if the button state
////                      (PUSHED or RELEASED) has changed since the last call, otherwise returns
////                      NO_CHANGE.  The argument butName should be one of constants in the
////                      enumeration butStates, excluding 'NUM_BUTS'. Safe under interrupt.
//uint8_t
//checkButton (uint8_t butName);
//
//
//// *******************************************************
//// ButTask:             Function designed to be called regularly. It polls all
////                      buttons once and updates variables associated with the buttons if
////                      necessary.  It is efficient enough to be part of an ISR, e.g. from
////                      a SysTick interrupt.

typedef struct Switch_s {
    uint8_t status_init;
    uint8_t status_current;
} Switch;


void
initButtons(void);


void
initSwitches(void);

void
initSliderSwitch (void);

void
UpdateSliderSwitch (void);

void
updateButtons(void);

uint8_t
checkButton(uint8_t butName);

void
UpdateReferences(void);

void
ButtonTask(void *pvParameters);

void
vTimerCallback(TimerHandle_t xTimer);

void
initButtonTimer(void);

uint32_t
initButtonTask(void);

#endif /*BUTTONS_H_*/
