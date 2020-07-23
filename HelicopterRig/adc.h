/*
 * adc_functions.h (Header file for adc.c)
 *
 *  Created on: 28/03/2019
 *  Team_members: Abhimanyu Chhabra(99799242), Saranya Ramani(27179399), Jin Kim(79903309)
 */

#ifndef ADC_H_
#define ADC_H_




// Functions declarations //

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

#endif /* ADC_H_ */
