/*****************************************************************************

 ENCE 464 Heli Rig - altitude.h

 Author:            Nathan James (44005459)
                    Luke Trenberth (47277086)
                    Abhimanyu Chhabra (99799242)

 Last modified:     21.08.2020

Purpose:            Simple initializing of ADC which samples with ADC_CHANNEL9(AIN9 - PE4 on tiva board)).

                    The ADC is being used to calculate the altitude of the helicopter. ADC samples at 100 Hz (0.01s) and then
                    writes it to a circular buffer. ADCProcessorTrigger(ADC0_BASE, 3) gets the sample - it is being called in Heli_Main.c
                    in void SysTickIntHandler(void).

                    Functions used from week 4 lab ADCdemo1.c (Author:  P.J. Bones) are:
                    1. void ADCIntHandler(void) -  void adc_int_handler(void)
                    2. void initADC (void)      -  void init_adc (void)
 */

#ifndef ALTITUDE_H_
#define ALTITUDE_H_


#include <stdint.h>



//*****************************************************************************
// computeAltitude:         The handler for the ADC conversion complete interrupt.
//                          Receives values from the queue, summing the result
//
// RETURNS:                 The average ADC sum over the given cycles
int32_t
computeAltitude (void);



//*****************************************************************************
// percentAltitude:         Finds the initial position
//                          Calculates and returns the altitude percentage
//
// RETURNS:                 The percentage of altitude unbound
int32_t
percentAltitude(void);



//Initializes ADC at channel 9
//*****************************************************************************
// initADC:                 Intializes ADC_CHANNEL9(AIN9 - PE4 on tiva board)
//                          Based on Week 4 lab ADCdemo1.c - void initADC (void)
void
initADC (void);



//*****************************************************************************
// ADCTriggerTask:          FreeRTOS Task processing the ADC Trigger
void
ADCTriggerTask(void *pvParameters);



//*****************************************************************************
// ADCReceiveTask:          FreeRTOS Task updating the current altitude by calculation
void
ADCReceiveTask(void *p);



//*****************************************************************************
// initADCTriggerTask:        Initialise the FreeRTOS Task ADCTriggerTask
//                            Creates the ADC Queue
// RETURNS:                   1 if no errors, 0 if errors occurred
uint32_t
initADCTriggerTask(void);



//*****************************************************************************
// intADCReceiveTask:         Initialise the FreeRTOS Task ADCReceiveTask
//                            Creates the ADC Queue
// RETURNS:                   1 if no errors, 0 if errors occurred
uint32_t
intADCReceiveTask(void);

#endif /*ALTITUDE_H_*/
