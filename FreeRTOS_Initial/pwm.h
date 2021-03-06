/*****************************************************************************

 ENCE 464 Heli Rig - pwm.h

 Author:            Nathan James (44005459)
                    Luke Trenberth (47277086)
                    Abhimanyu Chhabra (99799242)

 Last modified:     21.08.2020

 Purpose:           Contains functions to initialise and control PWM on Helicopter Rigs using the TivaBoard

 */


#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>



// PWM configuration
#define MAIN_FREQ  200
#define TAIL_FREQ  200

#define MIN_MAIN_DUTY 10
#define MIN_TAIL_DUTY 10
#define MAX_TAIL_DUTY 70
#define MAX_MAIN_DUTY 80


#define PWM_DIVIDER_CODE   SYSCTL_PWMDIV_4
#define PWM_DIVIDER        4

//  PWM Hardware Details M0PWM7 (gen 3)
//  ---Main Rotor PWM: PC5, J4-05
#define PWM_MAIN_BASE        PWM0_BASE
#define PWM_MAIN_GEN         PWM_GEN_3
#define PWM_MAIN_OUTNUM      PWM_OUT_7
#define PWM_MAIN_OUTBIT      PWM_OUT_7_BIT
#define PWM_MAIN_PERIPH_PWM  SYSCTL_PERIPH_PWM0
#define PWM_MAIN_PERIPH_GPIO SYSCTL_PERIPH_GPIOC
#define PWM_MAIN_GPIO_BASE   GPIO_PORTC_BASE
#define PWM_MAIN_GPIO_CONFIG GPIO_PC5_M0PWM7
#define PWM_MAIN_GPIO_PIN    GPIO_PIN_5

//  PWM Hardware Details M1PWM5 (gen 2)
//  ---Tail Rotor PWM: PF1, J3-10
#define PWM_TAIL_BASE        PWM1_BASE
#define PWM_TAIL_GEN         PWM_GEN_2
#define PWM_TAIL_OUTNUM      PWM_OUT_5
#define PWM_TAIL_OUTBIT      PWM_OUT_5_BIT
#define PWM_TAIL_PERIPH_PWM  SYSCTL_PERIPH_PWM1
#define PWM_TAIL_PERIPH_GPIO SYSCTL_PERIPH_GPIOF
#define PWM_TAIL_GPIO_BASE   GPIO_PORTF_BASE
#define PWM_TAIL_GPIO_CONFIG GPIO_PF1_M1PWM5
#define PWM_TAIL_GPIO_PIN    GPIO_PIN_1



// Functions declarations

//*****************************************************************************
// init_pwm_clock:          Initialises the PWM clock
void
initPWMClock (void);



//*****************************************************************************
// initPWM:           Initializes two pwm signals
//                    M0PWM7 (J4-05, PC5) is used for the main rotor motor
//                    M1PWM5 (J3-10, PF1) is used for the tail rotor motor
void
initPWM (void);



//*****************************************************************************
// PWMOn:             Turns on PWM Output for both motors
void
PWMOn(void);



//*****************************************************************************
// PWMSetMain:         Sets the PWM on the main rotors
// INPUTS:             ui32Duty between 0-100, representing the Duty Cycle
void
PWMSetMain (uint32_t ui32Duty);



//*****************************************************************************
// PWMSetTail:         Sets the PWM on the tail rotor
// INPUTS:             ui32Duty between 0-100, representing the Duty Cycle
void
PWMSetTail(uint32_t ui32Duty);

//*****************************************************************************
// SetDuty:            Sets both the duty cycles of the PWM Signals
void
SetDuty (uint32_t main_duty,uint32_t tail_duty);

#endif /*PWM_H_*/
