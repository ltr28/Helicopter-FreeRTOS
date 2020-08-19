

/*
    adc.c

    Created on: 28/03/2019
    Team_members: Abhimanyu Chhabra(99799242), Saranya Ramani(27179399), Jin Kim(79903309)
 */

/*
    Simple initializing of ADC which samples with ADC_CHANNEL9(AIN9 - PE4 on tiva board)).

    The ADC is being used to calculate the altitude of the helicopter. ADC samples at 100 Hz (0.01s) and then
    writes it to a circular buffer. ADCProcessorTrigger(ADC0_BASE, 3) gets the sample - it is being called in Heli_Main.c
    in void SysTickIntHandler(void).

    Functions used from week 4 lab ADCdemo1.c (Author:  P.J. Bones) are:
    1. void ADCIntHandler(void) -  void adc_int_handler(void)
    2. void initADC (void)      -  void init_adc (void)
 */


#include "AllHeaderFiles.h"
#include "circBufT.h"
#include "pid.h"
#include "uart.h"

OperatingData_t OperatingData;
xSemaphoreHandle g_pUARTSemaphore;
BaseType_t xHigherPriorityTaskWoken;

#define BUF_SIZE 25
#define ADC_QUEUE_ITEM_SIZE sizeof(uint32_t)
#define RANGE_ALTITUDE  953

int32_t percentage;
int32_t initial_position;
bool init_land_alt = false;

QueueHandle_t xADCQueue;

/*
  The handler for the ADC conversion complete interrupt.
  Writes to the circular buffer. Based on Week 4 lab ADCdemo1.c - void ADCIntHandler(void)
 */

int32_t computeAltitude (void)
{
    //initiate the sum to be 0 and the altitude value to be zero
    int AltSum = 0;
    int i = 0;
    int32_t Altitude = 0;
    //For the size of Altitude Queue increment over it and add all values
    for(; i < BUF_SIZE; i++) {
        xQueueReceive(xADCQueue, &Altitude, portMAX_DELAY);    //Receive the queue and add it to Altitude Buffer
        AltSum = AltSum + Altitude;
    }
    return ((2 * AltSum + BUF_SIZE) / 2 / BUF_SIZE);    //returns an overall sum.
}

void resetAltitude(void)
{
    initial_position = computeAltitude();
}

int32_t percentAltitude(void)
{
     if (init_land_alt == false) {
         resetAltitude();
         init_land_alt = true;
     }
    int32_t current_position = computeAltitude();
    return (2*100*(initial_position-current_position)+RANGE_ALTITUDE)/(2*RANGE_ALTITUDE);
}

//// Place it in the circular buffer (advancing write index)
void
ADCIntHandler(void)
{
    xHigherPriorityTaskWoken = pdFALSE;
    uint32_t ulValue;
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);
    xQueueSendFromISR(xADCQueue, &ulValue,  &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);


}

/*
  Intializes ADC_CHANNEL9(AIN9 - PE4 on tiva board)
  Based on Week 4 lab ADCdemo1.c - void initADC (void)
 */
void
initADC (void)
{
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    SysCtlDelay(SysCtlClockGet()/12);

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

}

void ADCTriggerTask(void *pvParameters)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    while(1)
    {
        ADCProcessorTrigger(ADC0_BASE, 3); // Initiate a conversion
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(5));
    }
}



void ADCReceiveTask(void *p)
{

    TickType_t xTime;
    xTime = xTaskGetTickCount();

    while (1)
    {

        OperatingData.AltCurrent = percentAltitude();
//        UARTprintf("ALT: %d\n", (int) percentage);
        vTaskDelayUntil(&xTime, pdMS_TO_TICKS(10));
    }

}



uint32_t
initADCTriggerTask(void)
{
    xADCQueue = xQueueCreate( BUF_SIZE, ADC_QUEUE_ITEM_SIZE );
    if(xTaskCreate(ADCTriggerTask, (const portCHAR *)"Get Sample", 100, NULL,
                   tskIDLE_PRIORITY + PRIORITY_ALT_TASK, NULL) != pdTRUE)
    {
        return(1);
    }

    return(0);

}



uint32_t
intADCReceiveTask(void)
{

    if(xTaskCreate(ADCReceiveTask, (const portCHAR *)"Altitude_Calc", 100, NULL,
                   2, NULL) != pdTRUE)
    {
        return(1);
    }
    return(0);

}
