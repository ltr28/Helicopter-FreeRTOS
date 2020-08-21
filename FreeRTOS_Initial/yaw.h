/*****************************************************************************

 ENCE 464 Heli Rig - yaw.h

 Author:            Nathan James (44005459)
                    Luke Trenberth (47277086)
                    Abhimanyu Chhabra (99799242)

 Last modified:     21.08.2020

 Purpose:           Contains functions to initialise, read and update
                    the yaw position of the Helicopter

 */

#ifndef YAW_H_
#define YAW_H_


#include <stdint.h>




typedef struct Slots_s_t {
    int32_t current;
    int32_t mapped;
} Slots_s;



// *******************************************************
// Slots:                   Initializes the Slots variable and setting initial conditions
// RETURNS:                 The new Slots object
Slots_s initSlots (void);



// *******************************************************
// resetSlotState:          Resets the slot_state object parameters to 0
void
resetSlotState(void);



// *******************************************************
// EqualiseCurrentSlots:    Maps the slot states to the current value
void
EqualiseCurrentSlots(void);



// *******************************************************
// resetYaw:                Resets the slot number to 0
void
resetYaw (void);


// *******************************************************
// CalculateYaw:            Uses the current slot number on the disk to
//                          return an angle in degrees from the original reference point
void
CalculateYaw (void);

// *******************************************************
//  YawIntHandler:          Interrupt handler for the yaw interrupt.
//                          Measures Phasse A and Phase B.
//                          If moving clockwise, add 1 to slot
//                          If moving anti-clockwise, minus 1 to slot
void
YawIntHandler (void);



// *******************************************************
//  initYaw:                Interrupt initialisation for the yaw interrupt.
//                          Sets PB0 and PB1 to be inputs, enables interrupts on GPIOB.
//                          An interrupt occurs on both edges of PB0 and PB1 and when triggered,
//                          runs the YawIntHandler function
void
initYaw (void);



// *******************************************************
//  yawTask:                The FreeRTOS task for yaw control
void
yawTask (void *pvparameters);



// *******************************************************
//  inityawTask:            Initialise the FreeRTOS task yawTask
uint32_t
inityawTask(void);


#endif /* YAW_H_*/
