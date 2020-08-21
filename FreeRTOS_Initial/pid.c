#include "AllHeaderFiles.h"
#include "pid.h"


//*****************************************************************************
//    Control Constants:
//    KP_alt - Proportional gain for altitude
//    KI_alt - Integral gain for the altitude
//    KD_alt - Derivative gain for the altitude
//    KP_yaw - Proportional gain for yaw
//    KI_yaw - Integral gain for the yaw
//    KD_yaw - Derivative gain for the yaw

#define KP_alt 0.7
#define KI_alt 0.2
#define KD_alt 1
#define KP_yaw 0.6
#define KI_yaw 0.05
#define KD_yaw 1
#define DELTA_T 0.01



//*****************************************************************************
// pid_alt_init:            Initialises the Altitude PID constants
//                          to the Alt_PID structure
//
// RETURNS:                 PID_t containing initialised PID structure
PID_t pid_alt_init(void)
{
    PID_t Alt_PID;
    Alt_PID.k_p = KP_alt;
    Alt_PID.k_i = KI_alt;
    Alt_PID.k_d = KD_alt;

    Alt_PID.p_error = 0;
    Alt_PID.i_error = 0;
    Alt_PID.d_error = 0;

    Alt_PID.max_out = 90;
    Alt_PID.min_out = 10;

    Alt_PID.current = 0;
    Alt_PID.previous = 0;
    Alt_PID.delta_t = DELTA_T;
    Alt_PID.output = 0;

    return (Alt_PID);
}



//*****************************************************************************
// pid_yaw_init:            Initialises the Yaw PID constants
//                          to the Yaw_PID structure
//                          Initialises errors to 0
//                          Sets delta_t to DELTA_T
// RETURNS:                 PID_t containing initialised PID structure
PID_t pid_yaw_init(void)
{
    PID_t Yaw_PID;
    Yaw_PID.k_p = KP_yaw;
    Yaw_PID.k_i = KI_yaw;
    Yaw_PID.k_d = KD_yaw;

    Yaw_PID.p_error = 0;
    Yaw_PID.i_error = 0;
    Yaw_PID.d_error = 0;

    Yaw_PID.max_out = 90;
    Yaw_PID.min_out = 10;

    Yaw_PID.current = 0;
    Yaw_PID.previous = 0;
    Yaw_PID.delta_t = DELTA_T;
    Yaw_PID.output = 0;

    return (Yaw_PID);

}



//*****************************************************************************
// PIDUpdate:               Updates the given PID values based off signal error
//
// RETURNS:                 The updated PID structure, with updated values
PID_t PIDUpdate (PID_t PID, int32_t current, int32_t set)
{
    PID.current = current;
    PID.p_error = set - current;
    PID.i_error += PID.p_error * PID.delta_t;
    PID.d_error = (PID.current - PID.previous)/PID.delta_t;
    PID.output = (PID.p_error * PID.k_p) + (PID.i_error * PID.k_i) + (PID.d_error * PID.k_d);
    PID.previous = PID.current;
    if (PID.output < PID.min_out) {
        PID.output = PID.min_out;
    } else if (PID.output > PID.max_out) {
        PID.output = PID.max_out;
    }
    return (PID);
}



//*****************************************************************************
// PIDReset:                Resets the PID_controller errors to be 0
//
// RETURNS:                 The updated PID structure, with 0 errors
PID_t PIDReset (PID_t PID)
{
    PID.p_error = 0;
    PID.i_error = 0;
    PID.d_error = 0;
    PID.output = 0;
    return (PID);
}
