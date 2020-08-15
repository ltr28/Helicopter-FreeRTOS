#ifndef CONSTANTS_H_
#define CONSTANTS_H_

#include <AllHeaderFiles.h>

typedef struct OperatingData_s_t {
    int HeliMode; //mode;
    uint32_t AltRef;
    uint32_t AltCurrent;
    uint32_t YawRef;
    uint32_t YawCurrent;
} OperatingData_t;

static uint32_t
ui32IncrementBound (int32_t inputValue, uint32_t maxBound, uint32_t minBound, int32_t incrementValue);

void
updateAltRef(OperatingData_t* OperatingData, bool Increase);

void
updateYawRef(OperatingData_t* OperatingData, bool Increase);

void
updateAltCur(OperatingData_t* OperatingData, uint32_t NewAlt);



#endif /*CONSTANTS_H_*/
