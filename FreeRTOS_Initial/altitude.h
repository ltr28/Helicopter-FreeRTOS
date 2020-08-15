#ifndef ALTITUDE_H_
#define ALTITUDE_H_







// Functions declarations //

#include <stdint.h>

//Initializes ADC at channel 9
void
init_adc (void);

//Calculates the mean adc value of the heli altitude position and sets it to (0%). Runs at the start of the program.
void
calculate_landed_position(void);

//Continuously calculates the mean adc value of the heli and converts it into percentage from 0 to 100%.
//Uses void calculate_landed_position(void) as a reference.
void
calculate_mean_adc_and_percentage(void);

//return the current altitude of the heli
int8_t
get_percentage(void);



extern uint32_t AltTriggerTask(void);
extern uint32_t Altitude_calc(void);

#endif /*ALTITUDE_H_*/
