/*
    pid_control.h (Header file for control.c)

    Created on: 27/05/2019
    Team_members: Abhimanyu Chhabra(99799242), Saranya Ramani(27179399), Jin Kim(79903309)
 */

#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <data_control.h>
#include <stdint.h>


//*****************************************************************************
// OperatingData_init:      The OperatingData_init function, initialising the Operating Data structure
//                          with given states and values
// RETURNS:                 The Operating Data structure with set initial variables
OperatingData_t
OperatingData_init (void);



//*****************************************************************************
// YawRefHandler:           When the reference signal is triggered, the
//                          YawRefHandler reads the current yaw value. If the helicopter
//                          is not oriented already and the yaw variable has been set,
//                          the oriented state is set to True.
void
YawRefHandler(void);


//*****************************************************************************
// FlightFSM:               Sets the Controls in each flight mode
//                          Triggers flight mode changes.
//                          OperatingData structure is updated based on flight conditions
void
FlightFSM (void);


//*****************************************************************************
// ControlTask:             The FreeRTOS task controlling the helicopter control states, calling FlightFSM
//                          Runs continuously
void
ControlTask (void *pvparameters);

//*****************************************************************************
// ControlTask:             The  FreeRTOS task ControlTask initialisation
extern uint32_t
initControlTask (void);

#endif /* _CONTROL_H_ */
