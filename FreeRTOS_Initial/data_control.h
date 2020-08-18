#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include "AllHeaderFiles.h"

typedef enum  {LANDED,ORIENTATION,TAKEOFF,FLYING,LANDING} flight_modes_t;

typedef struct OperatingData_s_t {
    flight_modes_t HeliMode;
    int32_t AltRef;
    int32_t AltCurrent;
    int32_t YawRef;
    int32_t YawMapped;
    int32_t YawCurrent;
    int32_t AltDuty;
    int32_t YawDuty;
    bool HelicopterOrientated;
} OperatingData_t;




#endif /*CONSTANTS_H_*/
