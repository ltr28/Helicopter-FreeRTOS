 /*
    Heli_Main.c

    Created on: 28/03/2019
    Team_members: Abhimanyu Chhabra(99799242), Saranya Ramani(27179399), Jin Kim(79903309)

  */

/*
  Heli_Main.c resets, enables and initializes all the peripherals required for the project.
  Code for initializing the Clock, the SysTick, and the software reset is written in this file.
 */

#include "all_h.h"
#include "pwm.h"
#include "adc.h"
#include "yaw.h"
#include "pid_control.h"
#include "uart.h"


#define RESET_GPIO_BASE GPIO_PORTA_BASE
#define RESET_GPIO_PIN GPIO_PIN_6
#define SAMPLE_RATE_HZ 100 // frequency for ADC sampling, updating the values for 4 buttons (right,left,up,down) and incrementing (uint8_t tickCount)
uint8_t tickCount = 0;

void peripherals_reset(void)
{
    SysCtlPeripheralReset (SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralReset (UART_USB_PERIPH_UART);
    SysCtlPeripheralReset (UART_USB_PERIPH_GPIO);
    SysCtlPeripheralReset (PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralReset (PWM_TAIL_PERIPH_PWM);
    SysCtlPeripheralReset (SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralReset (SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralReset (SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralReset (SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralReset (SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralReset (SYSCTL_PERIPH_GPIOF);

}

void pheripherals_enable(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlPeripheralEnable(UART_USB_PERIPH_UART);
    SysCtlPeripheralEnable(UART_USB_PERIPH_GPIO);
    SysCtlPeripheralEnable(PWM_MAIN_PERIPH_PWM);
    SysCtlPeripheralEnable(PWM_TAIL_PERIPH_PWM);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

/*
   void SysTickIntHandler(void) is running at 100 Hz(10ms). Every 10ms the ADC gets the sample,
   the buttons are updated and tickCount is incremented
 */
void
SysTickIntHandler (void)
{
    updateButtons (); // Poll the buttons
    ADCProcessorTrigger(ADC0_BASE, 3); // Initiate a conversion
    tickCount++;
}

/*
    void init_clock_and_systick (void) initializes the clock at 20 MHz and the SysTick at 10ms
 */
void
init_clock_and_systick (void)
{
    // Set the clock rate to 20 MHz
    SysCtlClockSet (SYSCTL_SYSDIV_10 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN |
                    SYSCTL_XTAL_16MHZ);

    // Set up the period for the SysTick timer.  The SysTick timer period is
    // set as a function of the system clock.
    SysTickPeriodSet(SysCtlClockGet() / SAMPLE_RATE_HZ);


    // Register the interrupt handler
    SysTickIntRegister(SysTickIntHandler);


    // Enable interrupt and device
    SysTickIntEnable();
    SysTickEnable();
}

/*
   Initializing the reset pin as an input. Configured as weak pull up (active low).
 */
void
init_reset_pin (void)
{
    GPIODirModeSet(RESET_GPIO_BASE,
                   RESET_GPIO_PIN,
                   GPIO_DIR_MODE_IN);

    GPIOPadConfigSet(RESET_GPIO_BASE,
                     RESET_GPIO_PIN,
                     GPIO_STRENGTH_4MA,
                     GPIO_PIN_TYPE_STD_WPU);
}

/*
   void reset(void) resets the micro controller if the signal goes low
   on RESET_GPIO_PIN.
 */
void reset(void)
{
    if(GPIOPinRead(RESET_GPIO_BASE, RESET_GPIO_PIN) == 0)
    {
        SysCtlReset();
    }
}

int main(void)
{
    peripherals_reset (); // all the peripherals are reset
    pheripherals_enable (); // all the peripherals are enabled
    init_clock_and_systick ();
    init_pwm_clock();
    init_adc (); // initialize ADC at channel 9.
    init_yaw ();
    init_pwm ();
    initButtons ();  // initialises 4 pushbuttons (UP, DOWN, LEFT, RIGHT)
    //OLEDInitialise ();  // initialize the Orbit OLED display
    initialiseUSB_UART ();
    init_reset_pin ();
    init_slider_switch_and_yaw_reference_pins ();

    turn_on_pwm_output ();
    set_initial_value_of_slider_switch ();

  // Enable interrupts to the processor.
    IntMasterEnable ();

    SysCtlDelay (SysCtlClockGet ()/12); // delay was used because calculate_landed_position(); was not giving accurate results.
    calculate_landed_position ();


    while (1)
    {
        reset ();
        calculate_degrees ();
        calculate_mean_adc_and_percentage ();
        flight_modes_FSM ();

        if (tickCount >= 25) // if tickCount = 25 = 250ms  = 4Hz send the data over UART and set the tickCount back to zero
        {
            tickCount = 0;
            uart_display ();
        }


    }
}








