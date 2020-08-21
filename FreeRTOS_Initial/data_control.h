#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include "AllHeaderFiles.h"

//*****************************************************************************
// flight_modes_t:          Creating the flight modes type
//                          There are five flight modes:
//                          1.Landed (* The program will initially start in LANDED mode regardless of slider switch position)
//                          2.Orientation
//                          3.Take off
//                          4.Flying
//                          5.Landing
typedef enum  {LANDED,ORIENTATION,TAKEOFF,FLYING,LANDING} flight_modes_t;


//*****************************************************************************
// OperatingData_s_t:       Creating the OperatingData_t type
//
// HeliMode:                Current helicopter state
// AltRef:                  Altitude Reference
// AltCurrent:              Current Altitude
// YawRef:                  Yaw Reference
// YawCurrentMapped:        Current Yaw Mapped between -180 to 180 degrees
//                          (referenced to starting yaw reference until the Helicopter is oriented)
// YawRefMapped:            Yaw Reference Mapped between -180 to 180 degrees
// YawCurrent:              Current Yaw Position unmapped (used for control)
// AltDuty:                 Altitude PWM Duty Cycle
// YawDuty:                 Yaw PWM Duty Cycle
// HelicopterOrientated:    Helicopter Orientation state.
typedef struct OperatingData_s_t {
    flight_modes_t HeliMode;
    int32_t AltRef;
    int32_t AltCurrent;
    int32_t YawRef;
    int32_t YawCurrentMapped;
    int32_t YawRefMapped;
    int32_t YawCurrent;
    int32_t AltDuty;
    int32_t YawDuty;
    bool HelicopterOrientated;

} OperatingData_t;




#endif /*CONSTANTS_H_*/
