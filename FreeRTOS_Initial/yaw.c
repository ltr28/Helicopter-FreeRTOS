
#include "AllHeaderFiles.h"
#include "buttons4.h"

#define degrees_in_circle 360
#define slots_in_rig_circle 448 //slots in rig circle*4 = 112*4  = 448. Because 4 interrupts generated per pulse


extern xSemaphoreHandle g_pUARTSemaphore;
extern xSemaphoreHandle g_pDataSemaphore;
extern OperatingData_t OperatingData;

typedef enum {STATE1 = 0, STATE2 = 2, STATE3 = 3, STATE4 = 1} yawStates_t ;  // 0b00 0b10 0b11 0b01

yawStates_t previous_yaw_State = STATE1; //set to correct state during initialization
yawStates_t current_yaw_state = STATE1;

int32_t current_slot_count = 0;
int32_t mapped_slot_count = 0; //stays within 448 to -448



// *******************************************************
// getYaw:          Uses the current slot number on the disk to
//                  return an angle in degrees from the original reference point.
// RETURNS:         Angle value between -180 < Yaw < 180 degrees.
void calculate_degrees(void)
{
    if((mapped_slot_count >= slots_in_rig_circle) || (mapped_slot_count <= -slots_in_rig_circle)) /*
                                                                                                     if mapped slot count goes beyond
                                                                                                     448 or -448 set it back to 0.*/

    {
        mapped_slot_count= 0;
    }

    OperatingData.YawCurrent =  (2*current_slot_count*degrees_in_circle + slots_in_rig_circle) / 2 / slots_in_rig_circle;
    OperatingData.YawCurrentMapped = (2*mapped_slot_count*degrees_in_circle + slots_in_rig_circle) / 2 / slots_in_rig_circle;
    /*  mapped degrees stays within 360 to -360 for
        displaying purposes
     */
}


// *******************************************************
// resetYaw:        Resets the slot number to 0
void resetYaw (void) {
    mapped_slot_count = 0;
    current_slot_count = 0;
}


// *******************************************************
//  YawIntHandler:  Interrupt handler for the yaw interrupt.
//                  Measures Phasse A and Phase B.
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
            current_slot_count--;
            mapped_slot_count--;
            break;
        case STATE2:
            current_slot_count++;
            mapped_slot_count++;
            break;
        }
        break;

        case STATE2:
            switch(current_yaw_state)
            {
            case STATE1:
                current_slot_count--;
                mapped_slot_count--;
                break;
            case STATE3:
                current_slot_count++;
                mapped_slot_count++;
                break;
            }
            break;

            case STATE3:
                switch(current_yaw_state)
                {
                case STATE2:
                    current_slot_count--;
                    mapped_slot_count--;
                    break;
                case STATE4:
                    current_slot_count++;
                    mapped_slot_count++;
                    break;
                }
                break;

                case STATE4:
                    switch(current_yaw_state)
                    {
                    case STATE3:
                        current_slot_count--;
                        mapped_slot_count--;
                        break;
                    case STATE1:
                        current_slot_count++;
                        mapped_slot_count++;
                        break;
                    }
                    break;
    }
    previous_yaw_State = current_yaw_state; //set the current state to the previous state
}


// *******************************************************
//  YawIntHandler: Interrupt initialisation for the yaw interrupt.
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

int32_t get_current_slot_count(void)
{
    return current_slot_count;
}

int32_t get_mapped_slot_count(void)
{
    return mapped_slot_count;
}

void set_current_slot_count(int32_t set_count)
{
    current_slot_count = set_count;
}

void set_mapped_slot_count(int32_t set_count)
{

    mapped_slot_count = set_count;
}

/*
   void find_yaw_ref(void) finds the reference yaw position. This is simply achieved by rotating the
   helicopter at a constant duty cycle for the tail and main motors and checking if the
   reference signal has gone low (Active low configuration). If the signal goes low  the current degrees is set to zero.
   Called in case ORIENTATION: - void flight_modes_FSM (void).
 */



void yawTask (void *pvparameters)
{

    TickType_t xTime;
    xTime = xTaskGetTickCount();
    while(1)

    {
        if((mapped_slot_count >= slots_in_rig_circle) || (mapped_slot_count <= -slots_in_rig_circle)) {
            mapped_slot_count = 0;
            //If mapped slot count goes beyond 448 or -448 set it back to 0
        }
        OperatingData.YawCurrent =  (2*current_slot_count*degrees_in_circle + slots_in_rig_circle) / 2 / slots_in_rig_circle;
        OperatingData.YawCurrentMapped = (2*mapped_slot_count*degrees_in_circle + slots_in_rig_circle) / 2 / slots_in_rig_circle;
        //Mapped degrees stays within 360 to -360 for displaying purposes
        vTaskDelayUntil(&xTime, pdMS_TO_TICKS(10));


    }
}

uint32_t inityawTask(void)
{
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

