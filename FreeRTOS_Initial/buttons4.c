// *******************************************************
// 
// buttons4:        Support for a set of FOUR specific buttons on the Tiva/Orbit.
//                  ENCE361 sample code.
//                  The buttons are:  UP and DOWN (on the Orbit daughterboard) plus
//                  LEFT and RIGHT on the Tiva.
//
//Note:             pin PF0 (the pin for the RIGHT pushbutton - SW2 on
//                  the Tiva board) needs special treatment - See PhilsNotesOnTiva.rtf.
//
// P.J. Bones UCECE
// Last modified:  7.2.2018
// 
// *******************************************************

// *******************************************************

#include "AllHeaderFiles.h"

#include "inc/tm4c123gh6pm.h"  // Board specific defines (for PF0)
#include "buttons4.h"
#include "yaw.h"
#include "timers.h"
#include "control.h"

extern xSemaphoreHandle g_pUARTSemaphore;
#define BUT_TASK_STACK_SIZE     200

// *******************************************************
// Globals to module
// *******************************************************
static bool but_state[NUM_BUTS];    // Corresponds to the electrical state
static uint8_t but_count[NUM_BUTS];
static bool but_flag[NUM_BUTS];
static bool but_normal[NUM_BUTS];   // Corresponds to the electrical state
bool button_flag;

static TimerHandle_t timer;
volatile int mode_180 = 0;
int32_t set_alt_point = 0; // desired altitude

int32_t mapped_set_yaw_point = 0; // mapped_set_yaw_point stays within 360 to -360 unlike set_yaw_point. For display purposes.\

int32_t current_press = 0;
int32_t last_press = 0;


// *******************************************************
uint8_t slider_switch  = 0; // stores the current value of the slider switch
uint8_t slider_switch_init = 0; // stores the initial value of the slider switch when everything is initialized in int main(void)


#define SLIDER_SWITCH_GPIO_BASE GPIO_PORTA_BASE
#define SLIDER_SWITCH_GPIO_PIN GPIO_PIN_7
#define YAW_REFERENCE_GPIO_BASE GPIO_PORTC_BASE
#define YAW_REFERENCE_GPIO_PIN GPIO_PIN_4

// *******************************************************
// initButtons: Initialise the variables associated with the set of buttons
// defined by the constants in the buttons2.h header file.
void initButtons(void)
{
    int i;

    // UP button (active HIGH)
    SysCtlPeripheralEnable(UP_BUT_PERIPH);
    GPIOPinTypeGPIOInput(UP_BUT_PORT_BASE, UP_BUT_PIN);
    GPIOPadConfigSet(UP_BUT_PORT_BASE, UP_BUT_PIN, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPD);
    but_normal[UP] = UP_BUT_NORMAL;
    // DOWN button (active HIGH)
    SysCtlPeripheralEnable(DOWN_BUT_PERIPH);
    GPIOPinTypeGPIOInput(DOWN_BUT_PORT_BASE, DOWN_BUT_PIN);
    GPIOPadConfigSet(DOWN_BUT_PORT_BASE, DOWN_BUT_PIN, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPD);
    but_normal[DOWN] = DOWN_BUT_NORMAL;
    // LEFT button (active LOW)
    SysCtlPeripheralEnable(LEFT_BUT_PERIPH);
    GPIOPinTypeGPIOInput(LEFT_BUT_PORT_BASE, LEFT_BUT_PIN);
    GPIOPadConfigSet(LEFT_BUT_PORT_BASE, LEFT_BUT_PIN, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPU);
    but_normal[LEFT] = LEFT_BUT_NORMAL;
    // RIGHT button (active LOW)
    // Note that PF0 is one of a handful of GPIO pins that need to be
    // "unlocked" before they can be reconfigured.  This also requires
    //      #include "inc/tm4c123gh6pm.h"
    SysCtlPeripheralEnable(RIGHT_BUT_PERIPH);
    //---Unlock PF0 for the right button:
    GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
    GPIO_PORTF_CR_R |= GPIO_PIN_0; //PF0 unlocked
    GPIO_PORTF_LOCK_R = GPIO_LOCK_M;
    GPIOPinTypeGPIOInput(RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN);
    GPIOPadConfigSet(RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN, GPIO_STRENGTH_2MA,
                     GPIO_PIN_TYPE_STD_WPU);
    but_normal[RIGHT] = RIGHT_BUT_NORMAL;
    SysCtlDelay(SysCtlClockGet() / 12);

    for (i = 0; i < NUM_BUTS; i++)
    {
        but_state[i] = but_normal[i];
        but_count[i] = 0;
        but_flag[i] = false;
    }
}









/*
   void init_slider_switch_and_yaw_reference_pins(void) initializes the slider switch and the yaw reference gpio pins as an input.
   Configuration of the slider switch  - weak pull down
   Configuration of the yaw_reference  - weak pull up
 */
void initSwitches(void)
{
    GPIODirModeSet(YAW_REFERENCE_GPIO_BASE,
                   YAW_REFERENCE_GPIO_PIN,
                   GPIO_DIR_MODE_IN);

    GPIOPadConfigSet(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_GPIO_PIN,
                     GPIO_STRENGTH_4MA,
                     GPIO_PIN_TYPE_STD_WPU);

    GPIOIntTypeSet(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_GPIO_PIN , GPIO_LOW_LEVEL);
    GPIOIntEnable(YAW_REFERENCE_GPIO_BASE, YAW_REFERENCE_GPIO_PIN);
    GPIOIntRegister(YAW_REFERENCE_GPIO_BASE,YawRefHandler); //If interrupt occurs, run YawRefIntHandler
    IntEnable(INT_GPIOB); //Enable interrupts on B.


    GPIODirModeSet(SLIDER_SWITCH_GPIO_BASE,
                   SLIDER_SWITCH_GPIO_PIN,
                   GPIO_DIR_MODE_IN);


    GPIOPadConfigSet(SLIDER_SWITCH_GPIO_BASE,
                     SLIDER_SWITCH_GPIO_PIN,
                     GPIO_STRENGTH_4MA,
                     GPIO_PIN_TYPE_STD_WPD);
}
int8_t
get_alt_ref(void)
{
    return set_alt_point;

}
uint8_t
GetSliderSwitchInit (void)
{
    return slider_switch_init;
}

void
initSliderSwitch (void)
{
    slider_switch_init =  GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7);
}

uint8_t
SetSliderSwitch (void)
{
    slider_switch = GPIOPinRead(GPIO_PORTA_BASE, GPIO_PIN_7);
    return slider_switch;
}
// *******************************************************
// updateButtons: Function designed to be called regularly. It polls all
// buttons once and updates variables associated with the buttons if
// necessary.  It is efficient enough to be part of an ISR, e.g. from
// a SysTick interrupt.
// Debounce algorithm: A state machine is associated with each button.
// A state change occurs only after NUM_BUT_POLLS consecutive polls have
// read the pin in the opposite condition, before the state changes and
// a flag is set.  Set NUM_BUT_POLLS according to the polling rate.
void updateButtons(void)
{
    bool but_value[NUM_BUTS];
    int i;

    // Read the pins; true means HIGH, false means LOW
    but_value[UP] = (GPIOPinRead(UP_BUT_PORT_BASE, UP_BUT_PIN) == UP_BUT_PIN);
    but_value[DOWN] = (GPIOPinRead(DOWN_BUT_PORT_BASE, DOWN_BUT_PIN)
            == DOWN_BUT_PIN);
    but_value[LEFT] = (GPIOPinRead(LEFT_BUT_PORT_BASE, LEFT_BUT_PIN)
            == LEFT_BUT_PIN);
    but_value[RIGHT] = (GPIOPinRead(RIGHT_BUT_PORT_BASE, RIGHT_BUT_PIN)
            == RIGHT_BUT_PIN);
    // Iterate through the buttons, updating button variables as required
    for (i = 0; i < NUM_BUTS; i++)
    {
        if (but_value[i] != but_state[i])
        {
            but_count[i]++;
            if (but_count[i] >= NUM_BUT_POLLS)
            {
                but_state[i] = but_value[i];
                but_flag[i] = true;    // Reset by call to checkButton()
                but_count[i] = 0;
            }
        }
        else
            but_count[i] = 0;
    }
}

// *******************************************************
// checkButton: Function returns the new button logical state if the button
// logical state (PUSHED or RELEASED) has changed since the last call,
// otherwise returns NO_CHANGE.
uint8_t checkButton(uint8_t butName)
{
    if (but_flag[butName])
    {
        but_flag[butName] = false;
        if (but_state[butName] == but_normal[butName])
            return RELEASED;
        else
            return PUSHED;
    }
    return NO_CHANGE;
}

/*
 void setpoint_calculations(void) is used to change the desired altitude and the desired yaw according to the buttons pressed.
 UP button - desired altitude increases by 10%
 DOWN button - desired altitude decreases by 10%
 LEFT button - desired yaw decreases by 15 degrees
 RIGHT button - desired yaw increases by 15 degrees
 */

void setpoint_calculations(void)
{
    if (checkButton(UP) == PUSHED)
    {
        current_press = mode_180;
        if (current_press - last_press < 20) {
            set_alt_point = 50;
        }
        else{
            set_alt_point += 10;

            if (set_alt_point > 100) // if desired altitude is greater than or equal to 100% set it back to 100%.
            {
                set_alt_point = 100;
            }
        }
        last_press = current_press;
    }

    if (checkButton(DOWN) == PUSHED)
    {
        set_alt_point -= 10;

        if (set_alt_point < 0) // if desired altitude is less than 30%(stable altitude) set it back to 30%.
        {
            set_alt_point = 0;
        }
    }

    if (checkButton(LEFT) == PUSHED)
    {
        current_press = mode_180;
        if(current_press - last_press < 20 ) {
            set_yaw_point -= (180+15);
        }
        else{
            set_yaw_point -= 15;
            mapped_set_yaw_point -= 15;
        }
        last_press = current_press;
    }

    if (checkButton(RIGHT) == PUSHED)
    {
        current_press = mode_180;
        if(current_press - last_press < 20 ) {
            set_yaw_point += (180-15);
        }
        else{
            set_yaw_point += 15;
            mapped_set_yaw_point += 15;
        }
        last_press = current_press;
    }

    if (mapped_set_yaw_point >= 360 || mapped_set_yaw_point <= -360)
    {
        /*set the mapped yaw point back to zero if it goes beyond the range of 360 to -360 for displaying purposes*/
        mapped_set_yaw_point = 0;
    }

    //send values through Queue here
    //    xQueueSend(g_pAltQueue, &ulValue, &xHigherPriorityTaskWoken);
    //    xQueueSend(g_pQueue)
    //
    //    xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
    ////    UARTprintf("                          " );
    ////    UARTprintf("Map_Yaw = %i\n",mapped_set_yaw_point);
    ////    UARTprintf("Alt = %i\n", set_alt_point );
    ////    UARTprintf("                          " );
    //    xSemaphoreGive(g_pUARTSemaphore);
}

void ButtonTask(void *pvParameters)
{

    TickType_t xTime;
    xTime = xTaskGetTickCount();

    while (1)
    {
        setpoint_calculations();
        //yawQueue()
        vTaskDelayUntil(&xTime, pdMS_TO_TICKS(10));
    }
}

void vTimerCallback(TimerHandle_t xTimer)
{
    configASSERT( xTimer );
    updateButtons();
    mode_180 += 1;
//    if(mode_180 == 20)
//    {
//        mode_180 = 0;
//    }
}

void initButtonTimer(void)
{

    timer = xTimerCreate("Button_timer", pdMS_TO_TICKS(5), pdTRUE, (void *) 0,
                         vTimerCallback);
    if (timer == NULL)
    {
        while (1)
        {
            UARTprintf("Timer not created properly");
        }
    }
    else
    {
        /* Start the timer.  No block time is specified, and
     even if one was it would be ignored because the RTOS
     scheduler has not yet been started. */
        if (xTimerStart(timer, 0 ) != pdPASS)
        {
            /* The timer could not be set into the Active
         state. */
            UARTprintf("Timer not active");
        }
    }
}

uint32_t initButtonTask(void)
{
    //Create the task
    if (xTaskCreate(ButtonTask, (const portCHAR *) "BUT", BUT_TASK_STACK_SIZE, NULL,
                    tskIDLE_PRIORITY + PRIORITY_BUT_TASK, NULL) != pdTRUE)
    {
        return (1);
    }

    return (0);
}
