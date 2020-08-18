#ifndef PID_H_
#define PID_H_

#include <AllHeaderFiles.h>

typedef struct PID_s_t {
    int32_t k_p;
    int32_t k_i;
    int32_t k_d;

    int32_t p_error;
    int32_t i_error;
    int32_t d_error;

    int32_t max_out;
    int32_t min_out;

    int32_t current;
    int32_t previous;
    int32_t delta_t;
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


