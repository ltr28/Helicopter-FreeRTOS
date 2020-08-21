
#include "AllHeaderFiles.h"
#include <pwm.h>


//*****************************************************************************
// init_pwm_clock:          Initialises the PWM clock
void initPWMClock (void)
{
    SysCtlPWMClockSet(PWM_DIVIDER_CODE); // Set the PWM clock rate (using the prescaler)
}


//*****************************************************************************
// initPWM:           Initializes two pwm signals
//                    M0PWM7 (J4-05, PC5) is used for the main rotor motor
//                    M1PWM5 (J3-10, PF1) is used for the tail rotor motor
void initPWM (void)
{
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
    SysCtlDelay(SysCtlClockGet()/12);
    GPIOPinConfigure(PWM_MAIN_GPIO_CONFIG);
    GPIOPinConfigure(PWM_TAIL_GPIO_CONFIG);

    GPIOPinTypePWM(PWM_MAIN_GPIO_BASE,
                   PWM_MAIN_GPIO_PIN);
    GPIOPinTypePWM(PWM_TAIL_GPIO_BASE,
                   PWM_TAIL_GPIO_PIN);

    PWMGenConfigure(PWM_MAIN_BASE, PWM_MAIN_GEN,
                    PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM_TAIL_BASE, PWM_TAIL_GEN,
                        PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    PWMGenEnable(PWM_MAIN_BASE, PWM_MAIN_GEN);
    PWMGenEnable(PWM_TAIL_BASE, PWM_TAIL_GEN);


    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, false);

    // Disable the output.  Repeat this call with 'true' to turn O/P on.
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, false);
}

//*****************************************************************************
// PWMOn:             Turns on PWM Output for both motors
void PWMOn(void)
{
    // Initialisation is complete, so turn on the output.
    PWMOutputState(PWM_MAIN_BASE, PWM_MAIN_OUTBIT, true);
    PWMOutputState(PWM_TAIL_BASE, PWM_TAIL_OUTBIT, true);
}

//*****************************************************************************
// PWMSetMain:         Sets the PWM on the main rotors
// INPUTS:             ui32Duty between 0-100, representing the Duty Cycle
void PWMSetMain (uint32_t ui32Duty)
{
    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period =
            SysCtlClockGet() / PWM_DIVIDER / MAIN_FREQ;

    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM,
                     ui32Period * ui32Duty / 100);
}

//*****************************************************************************
// PWMSetTail:         Sets the PWM on the tail rotor
// INPUTS:             ui32Duty between 0-100, representing the Duty Cycle
void PWMSetTail(uint32_t ui32Duty)
{
    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period =
            SysCtlClockGet() / PWM_DIVIDER / TAIL_FREQ;

    PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32Period);
    PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM,
                     ui32Period * ui32Duty / 100);
}


//*****************************************************************************
// SetDuty:            Sets both the duty cycles of the PWM Signals
void SetDuty(uint32_t main_duty,uint32_t tail_duty)
{
    OperatingData.AltDuty = main_duty;
    OperatingData.YawDuty = tail_duty;
    PWMSetMain(main_duty);
    PWMSetTail(tail_duty);
}
