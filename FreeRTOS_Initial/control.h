/*
    pid_control.h (Header file for pid_control.c)

    Created on: 27/05/2019
    Team_members: Abhimanyu Chhabra(99799242), Saranya Ramani(27179399), Jin Kim(79903309)
 */

#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <stdint.h>

OperatingData_t
OperatingData_init (void);

// Functions declarations
void
YawRefHandler(void);

// sets the duty cycle for the main and the tail motors
void
SetDuty (uint32_t main_duty,uint32_t tail_duty);

void
FlightFSM (void);

void
ControlTask (void *pvparameters);


extern uint32_t
initControlTask (void);

#endif /* _CONTROL_H_ */
