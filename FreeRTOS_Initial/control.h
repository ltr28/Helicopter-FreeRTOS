/*
    pid_control.h (Header file for pid_control.c)

    Created on: 27/05/2019
    Team_members: Abhimanyu Chhabra(99799242), Saranya Ramani(27179399), Jin Kim(79903309)
 */

#ifndef _CONTROL_H_
#define _CONTROL_H_

#include <stdint.h>


// Functions declarations

// sets the initial value for the slider switch in int main(void)
void
set_initial_value_of_slider_switch (void);

// initializes the slider switch and the yaw reference gpio pins
void
init_slider_switch_and_yaw_reference_pins (void);


extern uint32_t
initControlTask (void);

#endif /* _CONTROL_H_ */
