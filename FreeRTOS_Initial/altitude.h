#ifndef ALTITUDE_H_
#define ALTITUDE_H_







// Functions declarations //

#include <stdint.h>

int32_t
computeAltitude (void);

void
resetAltitude (void);
//Initializes ADC at channel 9
void
initADC (void);

//Calculates the mean adc value of the heli altitude position and sets it to (0%). Runs at the start of the program.
void
calculate_landed_position(void);

//Continuously calculates the mean adc value of the heli and converts it into percentage from 0 to 100%.
//Uses void calculate_landed_position(void) as a reference.
void
calculate_mean_adc_and_percentage(void);



extern uint32_t initADCTriggerTask(void);
extern uint32_t intADCReceiveTask(void);

#endif /*ALTITUDE_H_*/
