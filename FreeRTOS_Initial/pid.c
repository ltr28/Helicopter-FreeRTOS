#include "AllHeaderFiles.h"
#include "pid.h"


/* _alt - main_rotor
   KP_alt - Proportional gain for altitude
   KI_alt - Integral gain for the altitude
   KD_alt - Derivative gain for the altitude
 */
#define KP_alt 2.5
#define KI_alt 0.5
#define KD_alt 0


/* _yaw - tail_rotor
    KP_yaw - Proportional gain for yaw
   KI_yaw - Integral gain for the yaw
   KD_yaw - Derivative gain for the yaw
 */
#define KP_yaw 1.5
#define KI_yaw 0.5
#define KD_yaw 0
#define DELTA_T 0.01

PID_t
pid_alt_init(void)
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

PID_t
pid_yaw_init(void)
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

PID_t
PIDUpdate (PID_t PID, int32_t current, int32_t set)
{
    PID.current = current;
    PID.p_error = set - current;
    PID.i_error += PID.p_error * PID.delta_t;
    PID.d_error = (PID.current - PID.previous)*PID.delta_t;
    PID.output = (PID.p_error * PID.k_p) + (PID.i_error * PID.k_i) + (PID.d_error * PID.k_d);
    PID.previous = PID.current;
    if (PID.output < PID.min_out) {
        PID.output = PID.min_out;
    } else if (PID.output > PID.max_out) {
        PID.output = PID.max_out;
    }
    return (PID);
}

PID_t
PIDReset (PID_t PID)
{
    PID.p_error = 0;
    PID.i_error = 0;
    PID.d_error = 0;
    PID.output = 0;
    return (PID);
}
