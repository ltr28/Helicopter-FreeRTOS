//*****************************************************************************
//
// yaw - Calculating yaw slot numbers and angles functions through an Interrupt
//
// Author:  N. James
//          L. Trenberth
//          M. Arunchayanon
// Last modified:   31.5.2019
//*****************************************************************************
#include "AllHeaderFiles.h"
#include "control.h"
#include "motor.h"
#include "uart.h"
#include "yaw.h"


#define NUM_SLOTS               448
#define TOTAL_ANGLE             360
#define FIND_REF_MAIN           30 //duty cycle for finding the reference point
#define FIND_REF_TAIL           40

#define YAW_ITEM_SIZE sizeof(uint32_t)
#define YAW_QUEUE_SIZE 25
#define YAWTASKSTACKSIZE 1028
//#define MAX_STR_LEN 300


QueueHandle_t g_pYawQueue;
extern xSemaphoreHandle g_pUARTSemaphore;

// Sets quadrature encoding states A, B, C, D
enum quad {A = 0, B = 1, C = 3, D = 2};
enum quad State;
enum quad nextState;

//Sets the slot number, the number of slots moved around the disc.
int32_t slot;


// *******************************************************
// getYaw:          Uses the current slot number on the disk to
//                  return an angle in degrees from the original reference point.
// RETURNS:         Angle value between -180 < Yaw < 180 degrees.

int32_t getYaw(void) {
    int32_t angle = 0;
    int32_t refnum = slot;
    while (refnum > NUM_SLOTS / 2) {
        refnum -= NUM_SLOTS;
    }
    while (refnum < -NUM_SLOTS / 2) {
        refnum += NUM_SLOTS;
    }
//  Slots converted into an angle and returned as an angle.
    angle = TOTAL_ANGLE * refnum / NUM_SLOTS;
    return angle;
}


// *******************************************************
// resetYaw:        Resets the slot number to 0
void resetYaw (void) {
    slot = 0;
}


// *******************************************************
//  YawIntHandler:  Interrupt handler for the yaw interrupt.
//                  Measures Phasse A and Phase B.
//                  If moving clockwise, add 1 to slot
//                  If moving anti-clockwise, minus 1 to slot

void YawIntHandler (void) {
    //Clear the interrupt bits
    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1);

//  Sets nextState based off status of Pin 0 & 1
    nextState = (enum quad)GPIOPinRead(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    switch(State)
    {
    case A:
        //In case A, can move to B or D.
        //D is clockwise, B is anti-clockwise
        switch (nextState)
        {
            case B:
                slot--;
                break;
            case D:
                slot++;
                break;
        }
        break;
    case B:
        //In case B, can move to A or C.
        //A is clockwise, C is anti-clockwise
        switch (nextState)
        {
        case C:
            slot--;
            break;
        case A:
            slot++;
            break;
        }
        break;
    case C:
    {
        //In case C, can move to D or B.
        //B is clockwise, D is anti-clockwise
        switch(nextState)
        {
        case D:
            slot--;
            break;
        case B:
            slot++;
            break;
        }
        break;
    }
    case D:
    {
        //In case D, can move to A or C.
        //C is clockwise, A is anti-clockwise
        switch(nextState)
        {
        case A:
            slot--;
            break;
        case C:
            slot++;
            break;
        }
        break;
    }
    }
    State = nextState;
}

// *******************************************************
//  YawIntHandler: Interrupt initialisation for the yaw interrupt.
//                 Sets PB0 and PB1 to be inputs, enables interrupts on GPIOB.
//                 An interrupt occurs on both edges of PB0 and PB1 and when triggered,
//                 runs the YawIntHandler function
void initYaw (void) {

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    GPIOPinTypeGPIOInput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1); //Sets PIN) & PIN1 as inputs
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_BOTH_EDGES); //Trigger interrupts on both edges of wave changes on PB0 and PB1
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0 | GPIO_INT_PIN_1); //Enable interrupts from PB0 and PB1
    GPIOIntRegister(GPIO_PORTB_BASE, YawIntHandler); //If interrupt occurs, run YawIntHandler
    IntEnable(INT_GPIOB); //Enable interrupts on B.
}

void yawTask (void *pvparameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    resetYaw();
    for( ;; ) {
        GPIOIntRegister(GPIO_PORTB_BASE, YawIntHandler); //If interrupt occurs, run YawIntHandler
        //adding the getYaw to a queue
        int32_t yaw = getYaw();
        UARTFormat(yaw, "Current Yaw: %d \r\n");
        //int32_t yaw = 0;
//        char theYawStr[MAX_STR_LEN+1];
//        //Combine the integer with a string so that it can be printed to UART
//        usprintf(theYawStr, "Current Yaw is %d degrees\r\n", yaw);
//        //Give and take the semaphore and print it out on UART
//        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
//        UARTSend(theYawStr);
//        xSemaphoreGive(g_pUARTSemaphore);
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
    }
}

uint32_t initYawTask(void) {

    //Set initial state conditions
    g_pYawQueue = xQueueCreate(YAW_QUEUE_SIZE, YAW_ITEM_SIZE);

    if(xTaskCreate(yawTask, (const portCHAR *)"YAW", YAWTASKSTACKSIZE, NULL, tskIDLE_PRIORITY + PRIORITY_YAW_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    return (0);
}

