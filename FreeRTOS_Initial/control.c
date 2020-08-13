/*
    pid_control.c

    Created on: 27/05/2019
    Team_members: Abhimanyu Chhabra(99799242), Saranya Ramani(27179399), Jin Kim(79903309)
 */

/*
   pid_control.c file contains PID control functions which are required to
   maintain the desired altitude and the desired yaw of the helicopter.

   The pid update functions for the altitude and the yaw in this file are based on the pid update
   example which was given in PID 3 lecture notes of ENCE 361.
 */

#include <stdint.h>
#include "AllHeaderFiles.h"
#include "altitude.h"
#include "yaw.h"
#include "pwm.h"
#include "buttons4.h"

int32_t current_slot_count = 0;
int32_t mapped_slot_count = 0; //stays within 448 to -448
int32_t actual_degrees = 0;
int32_t mapped_degrees = 0; // stays within 360 to -360

/* _alt - main_rotor
   KP_alt - Proportional gain for altitude
   KI_alt - Integral gain for the altitude
   KD_alt - Derivative gain for the altitude
 */
#define KP_alt 0.5
#define KI_alt 0.08
#define KD_alt 0.6

/* _yaw - tail_rotor
    KP_yaw - Proportional gain for yaw
   KI_yaw - Integral gain for the yaw
   KD_yaw - Derivative gain for the yaw
 */
#define KP_yaw 1.2
#define KI_yaw 0.05
#define KD_yaw 1.2

#define YAW_REFERENCE_GPIO_BASE GPIO_PORTC_BASE
#define SLIDER_SWITCH_GPIO_BASE GPIO_PORTA_BASE
#define YAW_REFERENCE_GPIO_PIN GPIO_PIN_4
#define SLIDER_SWITCH_GPIO_PIN GPIO_PIN_7


typedef enum  {LANDED,ORIENTATION,TAKEOFF,FLYING,LANDING} flight_modes_t;
flight_modes_t current_fligt_mode = LANDED;
bool first = false;


const double delta_time = 0.01; // delta time is the same as the sampling time of the adc.
double alt_error = 0;
double alt_last_error = 0;
double alt_integrated_error = 0;

double yaw_error = 0;
double yaw_last_error = 0;
double yaw_integrated_error = 0;


int32_t yaw_duty = 0; // stores the duty cycle of the tail rotor
int32_t alt_duty = 0; // stores the duty cycle of the main rotor


uint8_t current_slider_switch_value  = 0; // stores the current value of the slider switch
uint8_t initial_slider_switch_value = 0; // stores the initial value of the slider switch when everything is initialized in int main(void)

void
set_initial_value_of_slider_switch (void)
{
    initial_slider_switch_value =  GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7);
}


/*
   void find_yaw_ref(void) finds the reference yaw position. This is simply achieved by rotating the
   helicopter at a constant duty cycle for the tail and main motors and checking if the
   reference signal has gone low (Active low configuration). If the signal goes low  the current degrees is set to zero.
   Called in case ORIENTATION: - void flight_modes_FSM (void).
 */
void YawRefHandler(void)
{
    GPIOIntClear(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_GPIO_PIN); // clears the interrupt flag
    int yawref  =  GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);

    if(first == false)
    {
        if (yawref == 0)
        {
            first = true;
        }

        else
        {

        }

    }
}



/*
   void init_slider_switch_and_yaw_reference_pins(void) initializes the slider switch and the yaw reference gpio pins as an input.
   Configuration of the slider switch  - weak pull down
   Configuration of the yaw_reference  - weak pull up
 */
void init_slider_switch_and_yaw_reference_pins(void)
{


    GPIODirModeSet(YAW_REFERENCE_GPIO_BASE,
                   YAW_REFERENCE_GPIO_PIN,
                   GPIO_DIR_MODE_IN);

    GPIOPadConfigSet(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_GPIO_PIN,
                     GPIO_STRENGTH_4MA,
                     GPIO_PIN_TYPE_STD_WPU);

    GPIOIntTypeSet(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_GPIO_PIN , GPIO_LOW_LEVEL); //Trigger interrupts on both edges of wave changes on PB0 and PB1
    GPIOIntEnable(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_GPIO_PIN); //Enable interrupts from PB0 and PB1
    GPIOIntRegister(YAW_REFERENCE_GPIO_BASE,YawRefHandler); //If interrupt occurs, run YawIntHandler
    IntEnable(INT_GPIOB); //Enable interrupts on B.


    GPIODirModeSet(SLIDER_SWITCH_GPIO_BASE,
                   SLIDER_SWITCH_GPIO_PIN,
                   GPIO_DIR_MODE_IN);


    GPIOPadConfigSet(SLIDER_SWITCH_GPIO_BASE,
                     SLIDER_SWITCH_GPIO_PIN,
                     GPIO_STRENGTH_4MA,
                     GPIO_PIN_TYPE_STD_WPD);


}


/*
   double pid_alt_control_update(w,x,y,z) returns the duty cycle of the main motor to
   maintain the desired altitude based on pid.
 */
double
pid_alt_control_update (double alt_proportional_gain,
                        double alt_integral_gain,
                        double alt_derivative_gain,
                        double delta_t)
{
    double alt_derivative_error;
    double alt_control; // duty cycle for main motor

    alt_error = set_alt_point - get_percentage(); // subtract the current altitude from the desired altitude
    alt_integrated_error += alt_error * delta_t;
    alt_derivative_error = (alt_error - alt_last_error) / delta_t;
    alt_control = alt_error * alt_proportional_gain
            + alt_integrated_error * alt_integral_gain
            + alt_derivative_error * alt_derivative_gain;

    if (alt_control < MIN_MAIN_DUTY)
    {
        alt_control = MIN_MAIN_DUTY;
    }

    if (alt_control > MAX_MAIN_DUTY)
    {
        alt_control = MAX_MAIN_DUTY;
    }

    alt_last_error = alt_error;
    alt_duty = alt_control; // alt_duty is used for displaying the duty cycle of the main motor over uart. See void uart_display(void) - uart.c file
    return alt_control;
}

/*
   double pid_yaw_control_update(w,x,y,z) returns the duty cycle of the tail motor to
   maintain the desired yaw based on pid.
 */
double
pid_yaw_control_update (double yaw_proportional_gain,
                        double yaw_integral_gain,
                        double yaw_derivative_gain,
                        double delta_t)
{

    double yaw_derivative_error;
    double yaw_control; // duty cycle for tail motor

    yaw_error = set_yaw_point - actual_degrees;
    yaw_integrated_error += yaw_error * delta_t;
    yaw_derivative_error = (yaw_error - yaw_last_error) / delta_t;
    yaw_control = yaw_error * yaw_proportional_gain
            + yaw_integrated_error * yaw_integral_gain
            + yaw_derivative_error * yaw_derivative_gain;

    if(yaw_control < MIN_TAIL_DUTY)
    {
        yaw_control = MIN_TAIL_DUTY;
    }

    if(yaw_control > MAX_TAIL_DUTY)
    {
        yaw_control = MAX_TAIL_DUTY;
    }

    yaw_last_error = yaw_error;
    yaw_duty = yaw_control; // yaw_duty is used for displaying the duty cycle of the tail motor over uart. See void uart_display(void) - uart.c file
    return yaw_control;
}

/*
   void duty_cycle_based_on_pid (void) sets the duty cycle for the main motor and the tail motor based on pid update functions for  altitutde and yaw.
   Called in case TAKEOFF:, case FLYING: and case LANDING: - void flight_modes_FSM (void)
 */
void
duty_cycle_based_on_pid (void)
{

    set_duty_cycle_for_main_and_tail_motor(pid_alt_control_update(KP_alt, KI_alt, KD_alt, delta_time),
                                           pid_yaw_control_update(KP_yaw, KI_yaw, KD_yaw, delta_time));
}


/*
   void landing (void) is being used for the smooth landing of the helicopter.
   This function monitors that the altitude should drop by only 10%  if the helicopter is facing
   the reference yaw position(0 degrees) and the current altitude is same as the desired altitude.
   PID is used to control the duty cycle of both the tail and main motors.
   Called in case LANDING: - void flight_modes_FSM (void).
 */
void
landing (void)
{
    set_yaw_point = 0;
    mapped_set_yaw_point = 0;
    duty_cycle_based_on_pid();
    if(actual_degrees == 0)
    {
        if(get_percentage() == set_alt_point)
        {
            set_alt_point -= 10;

            if(set_alt_point < 0) // if the desired altitude goes below zero set it back to zero
            {
                set_alt_point = 0;
            }
        }
    }

    if(get_actual_degrees() == 0 && get_percentage() == 0) /*
                                                              Once the current altitude and current degrees are equal to zero,
                                                              go to landed mode.
     */
    {
        current_fligt_mode  = LANDED;
    }
}

/*
   void flight_modes_FSM (void) controls what helicopter does in each flight mode and what causes the helicopter to change the flight mode.
   There are five flight modes:
   1.Landed (* The program will initially start in LANDED mode regardless of slider switch position)
   2.Orientation
   3.Take off
   4.Flying
   5.Landing
 */
void
flight_modes_FSM (void)
{
    current_slider_switch_value = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7); // Read slider switch
    if(initial_slider_switch_value == 128 && current_slider_switch_value == 0) /*If the initial value of the slider switch is high and the current value
                                                                                  of the slider switch is low - (the slider switch has been pushed down),
                                                                                  so set the initial value to low. Go to case LANDED:*/
    {
        initial_slider_switch_value = 0;
    }

    switch(current_fligt_mode)
    {

    /*
       In ORIENTATION:
       1.Heli finds the yaw reference position which is facing the camera and that position
          is set to zero degrees.
       2.Heli goes to TAKE OFF mode once the yaw reference position is found
     */
    case ORIENTATION:
        if(first == false)
        {
            set_duty_cycle_for_main_and_tail_motor(10,15);
        }

        else if(first == true)
        {
            current_fligt_mode == TAKEOFF;
        }

        break;

        /*
       In TAKE OFF:
       1.Heli goes up to stable altitude of 30% facing the yaw reference position
       2.Goes to FLYING mode if the current altitude = 30% and the current degrees = 0.
         */
    case TAKEOFF:
        set_yaw_point = 0;
        duty_cycle_based_on_pid();

        if( actual_degrees == set_yaw_point)
        {
            if(get_percentage() == set_alt_point && set_alt_point < 50)
            {
                set_alt_point  += 10;
            }
        }

        if(actual_degrees() == set_yaw_point && get_percentage() == 50)
        {
            current_fligt_mode  = FLYING;
        }
        break;

        /*
      In FLYING:
      1.Buttons left, right, up and down can be operated to change the altitude and yaw via setpoint_calculations();.
      2.Goes to LANDING mode if the slider switch is pushed down.
         */
    case FLYING:

        duty_cycle_based_on_pid();
        get_percentage();


        if(current_slider_switch_value == 0)
        {
            current_slot_count = mapped_slot_count; /*
                                                               current_slot_count is set to mapped_slot_count which stays within 448 to -448. So the
                                                               actual degrees are set within 360 to -360. This is done so that the heli doesn't
                                                               rotate million times to come back to zero degrees because of pid.*/
            current_fligt_mode = LANDING;
        }
        break;

        /*
      In LANDING:
      1.Heli starts landing smoothly facing the yaw reference position
      2.If the current degrees and current altitude is equal to zero, the Heli goes into
        LANDED mode
         */
    case LANDING:
        landing();
        break;

        /*
      In LANDED:
      1.The duty cycle for both motors, the errors for pid update for altitude and yaw and the desired altitude and yaw are all set to zero
      2.Pushing the slider switch up will cause the helicopter to go either in Orientation mode or Take off mode.
         */
    case LANDED:
        set_duty_cycle_for_main_and_tail_motor(0,0);
        alt_error = 0;
        alt_last_error = 0;
        alt_integrated_error = 0;
        yaw_error = 0;
        yaw_last_error = 0;
        yaw_integrated_error = 0;
        set_yaw_point  = 0;
        set_alt_point = 0;
        mapped_set_yaw_point = 0;

        if(current_slider_switch_value == 128 && initial_slider_switch_value == 0) /*
                                                                                      The if statement prevents the heli from going into ORIENTATION mode
                                                                                      from landed mode if the slider switch is already pushed up at the start                                                                                      of the program.
         */
        {
            if(first == false)
            {
                current_fligt_mode = ORIENTATION; /*
                                                     current_flight_mode goes into orientation if yawref == true,
                                                     which means the reference has not been found. Once the reference
                                                     is found yawref is set to false.
                 */
            }

            else
            {
                current_fligt_mode = TAKEOFF; /*
                                                current_flight mode is set to take off mode if the yawref == false
                 */
            }
        }
        break;
    }
}

void control_task (void *pvparameters)
{
    TickType_t xTime;
    xTime = xTaskGetTickCount();

    while(1)
    {
        flight_modes_FSM();
        vTaskDelayUntil(&xTime, pdMS_TO_TICKS(10));
    }
}

uint32_t initControlTask (void)
{
    if(xTaskCreate(control_task(), (const portCHAR *)"Control_heli", 2000, NULL,
                   PRIORITY_CONTROL_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    return(0);
}
