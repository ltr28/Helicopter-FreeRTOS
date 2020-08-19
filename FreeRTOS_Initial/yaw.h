#ifndef YAW_H_
#define YAW_H_


#include <stdint.h>

typedef struct Slots_s_t {
    int32_t current;
    int32_t mapped;
} Slots_s;

void
resetSlotState(void);

void
EqualiseCurrentSlots(void);



// *******************************************************
// resetYaw:        Resets the slot number to 0
void
resetYaw (void);


void
CalculateYaw(void);

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



uint32_t
inityawTask(void);


#endif /* YAW_H_*/
