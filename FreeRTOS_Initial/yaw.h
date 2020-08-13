#ifndef YAW_H_
#define YAW_H_





#include <stdint.h>

int32_t get_actual_degrees(void);

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


// *******************************************************
// resetYaw:        Resets the slot number to 0
void
resetYaw (void);


// *******************************************************
//  YawIntHandler:  Interrupt handler for the yaw interrupt.
//                  Measures Phasse A and Phase B.
//                  If moving clockwise, add 1 to slot
//                  If moving anti-clockwise, minus 1 to slot
void
YawIntHandler (void);

// *******************************************************
//  YawIntHandler: Interrupt initialisation for the yaw interrupt.
//                 Sets PB0 and PB1 to be inputs, enables interrupts on GPIOB.
//                 An interrupt occurs on both edges of PB0 and PB1 and when triggered,
//                 runs the YawIntHandler function
void
initYaw (void);




extern uint32_t inityawTask(void);


#endif /* YAW_H_*/
