/*
    pid_control.h (Header file for pid_control.c)

    Created on: 27/05/2019
    Team_members: Abhimanyu Chhabra(99799242), Saranya Ramani(27179399), Jin Kim(79903309)
 */

#ifndef PID_CONTROL_H_
#define PID_CONTROL_H_


// Functions declarations

// sets the initial value for the slider switch in int main(void)
void
set_initial_value_of_slider_switch (void);

// initializes the slider switch and the yaw reference gpio pins
void
init_slider_switch_and_yaw_reference_pins (void);

// controls what heli does in each flight mode and how it switches from one flight mode to the other
void
flight_modes_FSM (void);

// return the current duty cycle for the tail motor
int32_t
get_yaw_duty (void);

// return the current duty cycle for the main motor
int32_t
get_alt_duty (void);

// returns the desired yaw
int32_t
get_mapped_set_yaw_point (void);

// returns the desired altitude
int32_t
get_set_alt_point (void);

// char* get_current_flight_mode(void) returns the current_flight_mode in string for sending it over the uart
char*
get_current_flight_mode (void);



#endif /* PID_CONTROL_H_ */
