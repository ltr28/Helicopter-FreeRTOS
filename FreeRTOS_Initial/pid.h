#ifndef PID_H_
#define PID_H_

#include <AllHeaderFiles.h>

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

PID_t
pid_alt_init(void);

PID_t
pid_yaw_init(void);

PID_t
PIDUpdate (PID_t PID, int32_t current, int32_t set);

PID_t
PIDReset (PID_t PID);


#endif /*PID_H_*/


