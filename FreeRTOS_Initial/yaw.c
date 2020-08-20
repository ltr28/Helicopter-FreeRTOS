
#include "AllHeaderFiles.h"
#include "yaw.h"
#include "buttons4.h"

#define TOTAL_DEGREES 360
#define TOTAL_SLOTS 448 //slots in rig circle*4 = 112*4  = 448. Because 4 interrupts generated per pulse


extern xSemaphoreHandle g_pUARTSemaphore;
extern xSemaphoreHandle g_pDataSemaphore;
extern OperatingData_t OperatingData;

typedef enum {STATE1 = 0, STATE2 = 2, STATE3 = 3, STATE4 = 1} yawStates_t ;  // 0b00 0b10 0b11 0b01

Slots_s slot_states;

Slots_s Slots (void)
{
    Slots_s Slots;
    Slots.current = 0; //Current Number of Slots
    Slots.mapped = 0; //stays within 448 to -448
    return (Slots);
}

void resetSlotState(void)
{
    slot_states.current = 0;
    slot_states.mapped = 0;
}

void EqualiseCurrentSlots(void)
{
    slot_states.current = slot_states.mapped;
}

yawStates_t previous_yaw_State = STATE1; //set to correct state during initialization
yawStates_t current_yaw_state = STATE1;





// *******************************************************
// getYaw:          Uses the current slot number on the disk to
//                  return an angle in degrees from the original reference point.
// RETURNS:         Angle value between -180 < Yaw < 180 degrees.
void CalculateYaw (void)
{
    if((slot_states.mapped >= TOTAL_SLOTS) || (slot_states.mapped <= -TOTAL_SLOTS)) /*
                                                                                                     if mapped slot count goes beyond
                                                                                                     448 or -448 set it back to 0.*/

    {
        slot_states.mapped= 0;
    }

    OperatingData.YawCurrent =  (2*slot_states.current*TOTAL_DEGREES + TOTAL_SLOTS) / 2 / TOTAL_SLOTS;
    OperatingData.YawCurrentMapped = (2*slot_states.mapped*TOTAL_DEGREES + TOTAL_SLOTS) / 2 / TOTAL_SLOTS;
    /*  mapped degrees stays within 360 to -360 for
        displaying purposes
     */
}

// *******************************************************
//  YawIntHandler:  Interrupt handler for the yaw interrupt.
//                  Measures Phase A and Phase B.
//                  If moving clockwise, add 1 to slot
//                  If moving anti-clockwise, minus 1 to slot
void YawIntHandler (void) {
    //Clear the interrupt bits
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_1); // clears the interrupt flag

    // Sneaky datatype trick - safe because pins are number 1 and 0
    // eg:
    // pins = 0b 0000 0011  => (decimal) 3 => STATE3
    current_yaw_state = (yawStates_t) GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_1|GPIO_PIN_0);

    // Take 'yawState', the condition before the interrupt
    // and from there decide what to do because of 'current_yaw_state'
    switch(previous_yaw_State)
    {

    case STATE1:
        switch(current_yaw_state)
        {
        case STATE4:
            slot_states.current--;
            slot_states.mapped--;
            break;
        case STATE2:
            slot_states.current++;
            slot_states.mapped++;
            break;
        }
        break;

        case STATE2:
            switch(current_yaw_state)
            {
            case STATE1:
                slot_states.current--;
                slot_states.mapped--;
                break;
            case STATE3:
                slot_states.current++;
                slot_states.mapped++;
                break;
            }
            break;

            case STATE3:
                switch(current_yaw_state)
                {
                case STATE2:
                    slot_states.current--;
                    slot_states.mapped--;
                    break;
                case STATE4:
                    slot_states.current++;
                    slot_states.mapped++;
                    break;
                }
                break;

                case STATE4:
                    switch(current_yaw_state)
                    {
                    case STATE3:
                        slot_states.current--;
                        slot_states.mapped--;
                        break;
                    case STATE1:
                        slot_states.current++;
                        slot_states.mapped++;
                        break;
                    }
                    break;
    }
    previous_yaw_State = current_yaw_state; //set the current state to the previous state
}


// *******************************************************
//  initYaw:       Interrupt initialisation for the yaw interrupt.
//                 Sets PB0 and PB1 to be inputs, enables interrupts on GPIOB.
//                 An interrupt occurs on both edges of PB0 and PB1 and when triggered,
//                 runs the YawIntHandler function
void initYaw (void) {

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    // while(!SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB));

    SysCtlDelay(SysCtlClockGet()/12);

    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1); //Sets PIN) & PIN1 as inputs
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);

    // Read the pins to get correct previous_yaw_State
    previous_yaw_State = (yawStates_t) GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_1|GPIO_PIN_0);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_BOTH_EDGES); //Trigger interrupts on both edges of wave changes on PB0 and PB1
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1); //Enable interrupts from PB0 and PB1
    GPIOIntRegister(GPIO_PORTB_BASE, YawIntHandler); //If interrupt occurs, run YawIntHandler
    IntEnable(INT_GPIOB); //Enable interrupts on B.
}


void yawTask (void *pvparameters)
{

    TickType_t xTime;
    xTime = xTaskGetTickCount();
    while(1)

    {
        if((slot_states.mapped >= TOTAL_SLOTS) || (slot_states.mapped <= -TOTAL_SLOTS)) {
            slot_states.mapped = 0;
            //If mapped slot count goes beyond 448 or -448 set it back to 0
        }
        xSemaphoreTake(g_pDataSemaphore, portMAX_DELAY);
        OperatingData.YawCurrent =  (2*slot_states.current*TOTAL_DEGREES + TOTAL_SLOTS) / 2 / TOTAL_SLOTS;
        OperatingData.YawCurrentMapped = (2*slot_states.mapped*TOTAL_DEGREES + TOTAL_SLOTS) / 2 / TOTAL_SLOTS;
        xSemaphoreGive(g_pDataSemaphore);
        //Mapped degrees stays within 360 to -360 for displaying purposes
        vTaskDelayUntil(&xTime, pdMS_TO_TICKS(10));


    }
}

uint32_t inityawTask(void)
{
    resetSlotState();
    if(xTaskCreate(yawTask, (const portCHAR *)"Get Yaw", YAW_TASK_STACK_SIZE, NULL,
                   PRIORITY_YAW_TASK, NULL) != pdTRUE)
    {
        return(1);
    }
    //
    // Success.
    //
    return(0);
}

