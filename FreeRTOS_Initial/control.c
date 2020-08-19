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

#include "AllHeaderFiles.h"
#include "yaw.h"
#include "pwm.h"
#include "buttons4.h"

extern xSemaphoreHandle g_pUARTSemaphore;
extern xSemaphoreHandle g_pDataSemaphore;

OperatingData_t OperatingData;
PID_t Alt_PID;
PID_t Yaw_PID;
Switch SliderSwitch;


OperatingData_t OperatingData_init (void)
{
    OperatingData_t OperatingData;
    OperatingData.HelicopterOrientated = false;
    OperatingData.AltRef = 0;
    OperatingData.YawRef = 0;
    OperatingData.AltCurrent = 0;
    OperatingData.YawCurrent = 0;
    OperatingData.YawCurrentMapped = 0;
    OperatingData.YawRefMapped = 0;
    OperatingData.HeliMode = LANDED;
    OperatingData.AltDuty = 20;
    OperatingData.YawDuty = 50;
    return (OperatingData_t) OperatingData;
}



void YawRefHandler(void)
{
    OperatingData.YawCurrent  =  GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);

    if(OperatingData.HelicopterOrientated == false)
    {
        if (OperatingData.YawRef == 0)
        {
            OperatingData.HelicopterOrientated = true;
        }
    }
    GPIOIntClear(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_GPIO_PIN); // clears the interrupt flag
    //
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
void FlightFSM (void)
{
    UpdateSliderSwitch();
    if(SliderSwitch.status_init == 128 && SliderSwitch.status_current == 0) {
        SliderSwitch.status_init = 0;
    } else if (SliderSwitch.status_init == 0 && SliderSwitch.status_current == 0) {
        OperatingData.HeliMode = LANDED;
    }
    /*If the initial value of the slider switch is high and the current value
                                                                                     of the slider switch is low - (the slider switch has been pushed down),
                                                                                     so set the initial value to low. Go to case LANDED:*/

    switch(OperatingData.HeliMode)
    {

    /*
       In ORIENTATION:
       1.Heli finds the yaw reference position which is facing the camera and that position
          is set to zero degrees.
       2.Heli goes to TAKE OFF mode once the yaw reference position is found
     */
    case ORIENTATION:
        OperatingData.AltDuty = 20;
        OperatingData.YawDuty = 60;
        OperatingData.YawRef = 0;
        OperatingData.AltRef = 20;
        Alt_PID.i_error = OperatingData.AltDuty / Alt_PID.k_i; //Sets the original output to be the Alt Duty, smoothing the transition from orientation
        Yaw_PID.i_error = OperatingData.YawDuty / Yaw_PID.k_i; //Sets the original output to be the Yaw Duty, smoothing the transition from orientation
        if(OperatingData.HelicopterOrientated == true)
        {
            resetYaw();
            OperatingData.HeliMode = TAKEOFF;

        }

        else
        {
            SetDuty(OperatingData.AltDuty, OperatingData.YawDuty);
        }

        break;

        /*
       In TAKE OFF:
       1.Heli goes up to stable altitude of 30% facing the yaw reference position
       2.Goes to FLYING mode if the current altitude = 30% and the current degrees = 0.
         */
    case TAKEOFF:
        Alt_PID = PIDUpdate(Alt_PID, OperatingData.AltCurrent, OperatingData.AltRef);
        Yaw_PID = PIDUpdate(Yaw_PID, OperatingData.YawCurrent, OperatingData.YawRef);
        SetDuty(Alt_PID.output, Yaw_PID.output);
        if((OperatingData.YawCurrent == OperatingData.YawRef) && (OperatingData.AltCurrent == 0)) {
            OperatingData.HeliMode  = FLYING;
        } break;

        /*
      In FLYING:
      1.Buttons left, right, up and down can be operated to change the altitude and yaw via setpoint_calculations();.
      2.Goes to LANDING mode if the slider switch is pushed down.
         */
    case FLYING:

        Alt_PID = PIDUpdate(Alt_PID, OperatingData.AltCurrent, OperatingData.AltRef);
        Yaw_PID = PIDUpdate(Yaw_PID, OperatingData.YawCurrent, OperatingData.YawRef);
        SetDuty(Alt_PID.output, Yaw_PID.output);


        if(SliderSwitch.status_current == 0) {
            set_current_slot_count(get_mapped_slot_count());
            OperatingData.HeliMode = LANDING;
/*
           current_slot_count is set to mapped_slot_count which stays within 448 to -448. So the
           actual degrees are set within 360 to -360. This is done so that the heli doesn't
           rotate million times to come back to zero degrees because of pid.*/
        } break;

        /*
      In LANDING:
      1.Heli starts landing smoothly facing the yaw reference position
      2.If the current degrees and current altitude is equal to zero, the Heli goes into
        LANDED mode
         */
    case LANDING:
    /*
       void landing (void) is being used for the smooth landing of the helicopter.
       This function monitors that the altitude should drop by only 10%  if the helicopter is facing
       the reference yaw position(0 degrees) and the current altitude is same as the desired altitude.
       PID is used to control the duty cycle of both the tail and main motors.
       Called in case LANDING: - void flight_modes_FSM (void).
     */
        OperatingData.YawRef = 0;
        OperatingData.YawCurrentMapped = 0;

        Alt_PID = PIDUpdate(Alt_PID, OperatingData.AltCurrent, OperatingData.AltRef);
        Yaw_PID = PIDUpdate(Yaw_PID, OperatingData.YawCurrent, OperatingData.YawRef);
        SetDuty(Alt_PID.output, Yaw_PID.output);

        if(OperatingData.AltCurrent == 0)
        {
            if(OperatingData.AltCurrent == OperatingData.AltRef)
            {
                OperatingData.AltRef -= 10;

                if(OperatingData.AltRef < 0) // if the desired altitude goes below zero set it back to zero
                {
                    OperatingData.AltRef = 0;
                }
            }
        }

        if(OperatingData.YawCurrent == 0 && OperatingData.AltCurrent == 0) /*
                                                                  Once the current altitude and current degrees are equal to zero,
                                                                  go to landed mode.
         */
        {
            OperatingData.HeliMode = LANDED;
        }
        break;

        /*
      In LANDED:
      1.The duty cycle for both motors, the errors for pid update for altitude and yaw and the desired altitude and yaw are all set to zero
      2.Pushing the slider switch up will cause the helicopter to go either in Orientation mode or Take off mode.
         */
    case LANDED:
        OperatingData.AltDuty = 0;
        OperatingData.YawDuty = 0;
        OperatingData.YawRef = 0;
        OperatingData.AltRef = 0;
        Alt_PID = PIDReset(Alt_PID);
        Yaw_PID = PIDReset(Yaw_PID);
        SetDuty(OperatingData.AltDuty, OperatingData.YawDuty);

        if(SliderSwitch.status_current == 128 && SliderSwitch.status_init == 0) {
          //The if statement prevents the heli from going into ORIENTATION mode
          //from landed mode if the slider switch is already pushed up at the start of the program.
            if(OperatingData.HelicopterOrientated == false) {
                OperatingData.HeliMode = ORIENTATION;
                // Current_flight_mode goes into orientation
                // Reference has not been found
                //Once the reference is found yawref is set to false.
            } else {
                OperatingData.HeliMode = TAKEOFF;
                //current_flight mode is set to take off mode
            }
        }
        break;
    }
}

void ControlTask (void *pvparameters)
{
    TickType_t xTime;
    xTime = xTaskGetTickCount();

    while(1)
    {
        FlightFSM();
        vTaskDelayUntil(&xTime, pdMS_TO_TICKS(10));
    }
}

uint32_t initControlTask (void)
{
    Alt_PID = pid_alt_init();
    Yaw_PID = pid_yaw_init();
    if(xTaskCreate(ControlTask, (const portCHAR *)"Control_heli", CONTROL_TASK_STACK_SIZE, NULL,
                   PRIORITY_CONTROL_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    return(0);
}
