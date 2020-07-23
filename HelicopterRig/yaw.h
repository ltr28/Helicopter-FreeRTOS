/*
    yaw_functions.h (Header file for yaw.c)

    Created on: 28/04/2019
    Team_members: Abhimanyu Chhabra(99799242), Saranya Ramani(27179399), Jin Kim(79903309)
 */

#ifndef YAW_H_
#define YAW_H_



// Functions declarations

// Initializes the gpio pins to calcultae the yaw for the heli
void
init_yaw(void);

// increments or decrements the current slot count once the interrupt is generated to calculate the yaw in void calculate_degrees(void).
void
yaw_int_handler(void);

void
calculate_degrees(void);

void
set_current_slot_count(int32_t set_count);

void
set_mapped_slot_count(int32_t set_count);

int32_t
get_actual_degrees(void);

int32_t
get_mapped_degress(void);

int32_t
get_current_slot_count(void);

int32_t
get_mapped_slot_count(void);


#endif /* YAW_H_ */
