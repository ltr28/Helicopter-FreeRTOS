/*
   yaw_functions.c

   Created on: 28/04/2019
   Team_members: Abhimanyu Chhabra(99799242), Saranya Ramani(27179399), Jin Kim(79903309)
 */

/*
    Simple initialize of the gpio pins  to generate interrupts at both edges.
    yaw.c is used to calculate the yaw when the heli rotates
 */

#include "all_h.h"

#define degrees_in_circle 360
#define slots_in_rig_circle 448 //slots in rig circle*4 = 112*4  = 448. Because 4 interrupts generated per pulse

typedef enum {STATE1 = 0, STATE2 = 2, STATE3 = 3, STATE4 = 1} yawStates_t ;  // 0b00 0b10 0b11 0b01
yawStates_t previous_yaw_State = STATE1; //set to correct state during initialization
yawStates_t current_yaw_state = STATE1;

int32_t current_slot_count = 0;
int32_t mapped_slot_count = 0; //stays within 448 to -448
int32_t actual_degrees = 0;
int32_t mapped_degrees = 0; // stays within 360 to -360

/*
   yaw_int_handler(void) uses a finite state machine which increments or decrements the current_slot_count depending
   on the previous and the current yaw state of the heli. Four possible states are:
   00 - 0
   01 - 1
   10 - 2
   11 - 3
 */
void yaw_int_handler(void)
{

    GPIOIntClear(GPIO_PORTB_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_1); // clears the interrupt flag

    // Sneaky datatype trick - safe because pins are number 1 and 0
    // eg:
    // pins = 0b 0000 0011  => (decimal) 3 => STATE3
    current_yaw_state = GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_1|GPIO_PIN_0);

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


/*
   initialized the gpio pins as inputs and generate interrupts at both edges
 */
void init_yaw(void)
{

    GPIODirModeSet(GPIO_PORTB_BASE,GPIO_PIN_0|GPIO_PIN_1,GPIO_DIR_MODE_IN);

    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPD);


    // Read the pins to get correct previous_yaw_State
    previous_yaw_State = GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_1|GPIO_PIN_0);

    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_BOTH_EDGES);
    GPIOIntEnable(GPIO_PORTB_BASE, GPIO_INT_PIN_0|GPIO_INT_PIN_1);
    GPIOIntRegister(GPIO_PORTB_BASE, yaw_int_handler);
}

/*
   display the current degrees on the Oled
 */
void display_degress(int angle)
{
    char string[17];
    usnprintf (string, sizeof(string), "yaw = %4d", angle);
    // Update line on display.
    OLEDStringDraw (string, 0, 3);
}

int32_t get_actual_degrees(void)
{
    return actual_degrees;
}

int32_t get_mapped_degress(void)
{
    return mapped_degrees;
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



void calculate_degrees(void)
{
    if((mapped_slot_count >= slots_in_rig_circle) || (mapped_slot_count <= -slots_in_rig_circle)) /*
                                                                                                     if mapped slot count goes beyond
                                                                                                     448 or -448 set it back to 0.
                                                                                                    */

    {
        mapped_slot_count= 0;
    }

    actual_degrees =  (2*current_slot_count*degrees_in_circle + slots_in_rig_circle) / 2 / slots_in_rig_circle;
    mapped_degrees = (2*mapped_slot_count*degrees_in_circle + slots_in_rig_circle) / 2 / slots_in_rig_circle; /*
                                                                                                                mapped degrees stays within 360 to -360 for
                                                                                                                displaying purposes
                                                                                                               */
}



