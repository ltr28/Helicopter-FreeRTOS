

#include "constants.h"
#include "AllHeaderFiles.h"

static uint32_t MAX_ALT_REF = 100;
static uint32_t MIN_ALT_REF = 0;
static uint32_t ALT_INCR = 10;
static uint32_t YAW_INCR = 15;



static uint32_t
ui32IncrementBound (int32_t inputValue, uint32_t maxBound, uint32_t minBound, int32_t incrementValue)
{
    uint32_t retVal;
    if ((inputValue + incrementValue > maxBound) || (inputValue + incrementValue < minBound)) {
            retVal = inputValue;
    }
    else {
        retVal = inputValue + incrementValue;
    }
    return (uint32_t) retVal;
}


void
updateAltRef (OperatingData_t* OperatingData, bool Increase)
{
    uint32_t oldAltRef = OperatingData->AltRef;
    uint32_t newAltRef;

    if (Increase)
    {
        newAltRef = ui32IncrementBound (oldAltRef, MAX_ALT_REF, MIN_ALT_REF, ALT_INCR);
    } else {
        newAltRef = ui32IncrementBound (oldAltRef, MAX_ALT_REF, MIN_ALT_REF, -ALT_INCR);
    }
    OperatingData->AltRef = newAltRef;
}


void
updateYawRef (OperatingData_t* OperatingData, bool Increase)
{
    uint32_t oldYawRef = OperatingData->YawRef;
    uint32_t newYawRef;

    if (Increase)
        {
            newYawRef = oldYawRef + YAW_INCR;
        } else {
            newYawRef = oldYawRef - YAW_INCR;
        }
        OperatingData->YawRef = newYawRef;
}

void
updateAltCur (OperatingData_t* OperatingData, uint32_t NewAlt)
{
    OperatingData->AltCurrent = NewAlt;
}

void
updateYawCur (OperatingData_t* OperatingData, uint32_t NewYaw)
{
    OperatingData->YawCurrent = NewYaw;
}



