#ifndef PID_H_
#define PID_H_

#include <AllHeaderFiles.h>


//*****************************************************************************
// PID_s_t:                 Creating the PID_t type
//
// k_p:                     The controller proportional gain
// k_i:                     The controller integral gain
// k_d:                     The controller differential gain

// p_error:                 The controller proportional error
// i_error:                 The controller integral error
// d_error:                 The controller differential error

// max_out:                 The controller maximum capped output
// min_out:                 The controller minimum capped output

// current:                 The current controller input
// previous:                The previous controller input
// output:                  The PID controller output capped between min_out and max_out

typedef struct PID_s_t {
    double k_p;
    double k_i;
    double k_d;
    double delta_t;

    double p_error;
    double i_error;
    double d_error;

    int32_t max_out;
    int32_t min_out;

    int32_t current;
    int32_t previous;
    int32_t output;

} PID_t;


//*****************************************************************************
// pid_alt_init:            Initialises the Altitude PID constants
//                          to the Alt_PID structure
//
// RETURNS:                 PID_t containing initialised PID structure
PID_t
pid_alt_init(void);


//*****************************************************************************
// pid_yaw_init:            Initialises the Yaw PID constants
//                          to the Yaw_PID structure
//                          Initialises errors to 0
//                          Sets delta_t to DELTA_T
// RETURNS:                 PID_t containing initialised PID structure
PID_t
pid_yaw_init(void);


//*****************************************************************************
// PIDUpdate:               Updates the given PID values based off signal error
//
// RETURNS:                 The updated PID structure, with updated values
PID_t
PIDUpdate (PID_t PID, int32_t current, int32_t set);


//*****************************************************************************
// PIDReset:                Resets the PID_controller errors to be 0
//
// RETURNS:                 The updated PID structure, with 0 errors
PID_t
PIDReset (PID_t PID);


#endif /*PID_H_*/


