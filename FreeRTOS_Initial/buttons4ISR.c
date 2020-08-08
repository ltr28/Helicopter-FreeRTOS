/*
 * buttons4ISR.c
 *
 *  Created on: 8/08/2020
 *      Author: ach227
 */

#include <AllHeaderFiles.h>

#include <inc/tm4c123gh6pm.h>
#include "buttons4ISR.h"

void
initButtons (void)
{

    // UP button (active HIGH)
    SysCtlPeripheralEnable (UP_BUT_PERIPH);
    GPIOPinTypeGPIOInput (UP_BUT_PORT_BASE, UP_BUT_PIN);

    GPIOPadConfigSet (UP_BUT_PORT_BASE, UP_BUT_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);
    but_normal[UP] = UP_BUT_NORMAL;
    // DOWN button (active HIGH)
    SysCtlPeripheralEnable (DOWN_BUT_PERIPH);
    GPIOPinTypeGPIOInput (DOWN_BUT_PORT_BASE, DOWN_BUT_PIN);
    GPIOPadConfigSet (DOWN_BUT_PORT_BASE, DOWN_BUT_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPD);
    but_normal[DOWN] = DOWN_BUT_NORMAL;
    // LEFT button (active LOW)
    SysCtlPeripheralEnable (LEFT_BUT_PERIPH);
    GPIOPinTypeGPIOInput (LEFT_BUT_PORT_BASE, LEFT_BUT_PIN);
    GPIOPadConfigSet (LEFT_BUT_PORT_BASE, LEFT_BUT_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPU);
    but_normal[LEFT] = LEFT_BUT_NORMAL;
    // RIGHT button (active LOW)
      // Note that PF0 is one of a handful of GPIO pins that need to be
      // "unlocked" before they can be reconfigured.  This also requires
      //      #include "inc/tm4c123gh6pm.h"
    SysCtlPeripheralEnable (RIGHT_BUT_PERIPH);
    //---Unlock PF0 for the right button:
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R |= GPIO_PIN_0; //PF0 unlocked
    GPIO_PORTF_LOCK_R = GPIO_LOCK_M;
    GPIOPinTypeGPIOInput (RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN);
    GPIOPadConfigSet (RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN, GPIO_STRENGTH_2MA,
       GPIO_PIN_TYPE_STD_WPU);
    but_normal[RIGHT] = RIGHT_BUT_NORMAL;

    GPIOIntTypeSet(UP_BUT_PORT_BASE|DOWN_BUT_PORT_BASE, UP_BUT_PIN|DOWN_BUT_PIN, GPIO_HIGH_LEVEL); //Trigger interrupts on both edges of wave changes on PB0 and PB1
    GPIOIntEnable((UP_BUT_PORT_BASE|DOWN_BUT_PORT_BASE|LEFT_BUT_PORT_BASE|RIGHT_BUT_PORT_BASE, UP_BUT_PIN|DOWN_BUT_PIN|RIGHT_BUT_PIN|LEFT_BUT_PIN); //Enable interrupts from PB0 and PB1
    GPIOIntRegister(RIGHT_BUT_PORT_BASE||LEFT_BUT_PORT_BASE|UP_BUT_PORT_BASE, YawIntHandler); //If interrupt occurs, run YawIntHandler
    IntEnable(INT_GPIOB); //Enable interrupts on B.
}


