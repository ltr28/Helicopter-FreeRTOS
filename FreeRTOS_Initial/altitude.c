#include <AllHeaderFiles.h>

#include "heliQueue.h"
#include "altitude.h"
#include "uart.h"

extern xSemaphoreHandle g_pUARTSemaphore;

#define ALTITUDETASKSTACKSIZE        128         // Stack size in words


#define ALT_ITEM_SIZE sizeof(uint32_t)
#define ALT_QUEUE_SIZE 25
#define RANGE_ALTITUDE  1051// mean adc value at (0% altitude - 100% altitude) = 993 approximately
//#define MAX_STR_LEN 100
#define ALTTASKSTACKSIZE 1028

//static uint16_t  landed_position = 0;
//static uint16_t  average = 0;
//static int8_t    percentage = 0;

QueueHandle_t g_pAltQueue;
int32_t initial_position = 0;

//bool initialRun = true;

//
///*

 //  *****************************************************************************
//  ADCIntHandler: The handler for the ADC conversion complete interrupt.
//                 Writes to the circular buffer.
//  Taken from:    Week4Lab ADCDemo1.c

void ADCIntHandler(void)
{
    ADCIntClear(ADC0_BASE, 3);
    uint32_t ulValue;
    int i;
    // Clean up, clearing the interrupt
    BaseType_t xHigherPriorityTaskWoken;

    /* We have not woken a task at the start of the ISR. */
    xHigherPriorityTaskWoken = pdFALSE;

    /* Loop until the buffer is empty. */
    for(i = 0; i < ALT_QUEUE_SIZE; i++) {
        // Get the single sample from ADC0.  ADC_BASE is defined in inc/hw_memmap.h
        ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);

        /* Post the byte. */
        xQueueSendFromISR(g_pAltQueue, &ulValue, &xHigherPriorityTaskWoken);
    }

    /* Now the buffer is empty we can switch context if necessary. */
    if (xHigherPriorityTaskWoken)
    {
        /* Actual macro used here is port specific. */
        taskYIELD();
    }
}


void
init_ADC (void)
{
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_ADC0));
    // Enable sample sequence 3 with a processor signal trigger.  Sequence 3
    // will do a single sample when the processor sends a signal to start the
    // conversion.
    ADCSequenceConfigure(ADC0_BASE, 3, ADC_TRIGGER_PROCESSOR, 0);

    // Configure step 0 on sequence 3.  Sample channel 0 (ADC_CTL_CH0) in
    // single-ended mode (default) and configure the interrupt flag
    // (ADC_CTL_IE) to be set when the sample is done.  Tell the ADC logic
    // that this is the last conversion on sequence 3 (ADC_CTL_END).  Sequence
    // 3 has only one programmable step.  Sequence 1 and 2 have 4 steps, and
    // sequence 0 has 8 programmable steps.  Since we are only doing a single
    // conversion using sequence 3 we will only configure step 0.  For more
    // on the ADC sequences and steps, refer to the LM3S1968 datasheet.
    ADCSequenceStepConfigure(ADC0_BASE, 3, 0, ADC_CTL_CH9 | ADC_CTL_IE |
                             ADC_CTL_END);

    // Since sample sequence 3 is now configured, it must be enabled.
    ADCSequenceEnable(ADC0_BASE, 3);

    // Register the interrupt handler
    ADCIntRegister (ADC0_BASE, 3, ADCIntHandler);

    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);

    //xTimerCreate

}

void AdcTrigger(void)
{
    ADCProcessorTrigger(ADC0_BASE, 3); // Initiate a conversion
}

/*
  Calculates the mean adc value when the helicopter is landed(0%). Calculated value is then used as a
  reference to calculate the altitude of the helicopter.

*/
//  *****************************************************************************
//  computeAltitude: Calculates the average altitude from the ADC.
//  Taken from:      Week4Lab ADCDemo1.c main function
//  RETURNS:         The calculated ADC altitude value as a int32_t

int32_t computeAltitude (void)
{
    //initate the sum to be 0 and the altitude value to be zero
    int AltSum, i = 0;
    int32_t Altitude = 0;
    //For the size of Altitude Queue increment over it and add all values
    for(; i < ALT_QUEUE_SIZE; i++) {
        xQueueReceive(g_pAltQueue, &Altitude, portMAX_DELAY);    //Recieve the queue and add it to Altitude Buffer
        AltSum = AltSum + Altitude;
    }
    return ((2 * AltSum + ALT_QUEUE_SIZE) / 2 / ALT_QUEUE_SIZE);    //returns an overall sum.
}

//  *****************************************************************************
//  resetAltitude: Resets the refAltitude to be current ADC altitude.
void resetAltitude (void)
{
    initial_position = computeAltitude();
}

//  *****************************************************************************
//  percentAltitude: Converts the ADC Altitude into a usable percentage altitude
//                   using a 0.8V difference as the maximum height
//  RETURNS:         A Height Percentage as a int32_t from the reference height.

int32_t percentAltitude(void)
{
    int32_t current_position = computeAltitude();
    //do some funky math that Abhimanyu Chhabara can explain pretty well
    return (2*100*(initial_position-current_position)+RANGE_ALTITUDE)/(2*RANGE_ALTITUDE); //returns percentage of 0.8V change
}

void AltTask(void *pvParameters)
{
    //get and set the last wake time to be the tick count
    TickType_t xLastWakeTime = xTaskGetTickCount();

    //Trigger the ADC
    AdcTrigger();

    //reset the altitude to be intial position (create a zero position)
    resetAltitude();

    //runs forever when task is scheduled by FreeRTOS
    for (;;) {
        //call the ADC trigger such that the altitude percentage is set to altitude
        AdcTrigger();
        int32_t altitude = percentAltitude();

        UARTFormat(altitude, "Current Altitude: %d \r\n");
//        char altitude_str[MAX_STR_LEN + 1];
//        //Combine the integer with a string so that it can be printed to UART
//        usprintf(altitude_str, "Current Altitude is %d \r\n", altitude);
//
//        //Give and take the semaphore and print it out on UART
//        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
//        UARTSend(altitude_str);
//        xSemaphoreGive(g_pUARTSemaphore);

        //delay the task until the correct amount of time has passed
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(100));
    }
}

uint32_t initAltTask(void) {

    //Set initial state conditions
    g_pAltQueue = xQueueCreate(ALT_QUEUE_SIZE, ALT_ITEM_SIZE);

    if(xTaskCreate(AltTask, (const portCHAR *)"ALT", ALTTASKSTACKSIZE, NULL, tskIDLE_PRIORITY + PRIORITY_ALT_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    return (0);
}

