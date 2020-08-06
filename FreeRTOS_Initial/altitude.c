//*****************************************************************************
//
// Altitude - File containing ADC and altitude calculations
//
// Author:  N. James
//          L. Trenberth
//          M. Arunchayanon
// Last modified:   31.5.2019
//
//*****************************************************************************




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


#include <AllHeaderFiles.h>
#include "circBufT.h"

extern xSemaphoreHandle g_pUARTSemaphore;

#define ALTITUDETASKSTACKSIZE        128         // Stack size in words
#define BUF_SIZE 25

static circBuf_t g_inBuffer; // Buffer of size BUF_SIZE integers (sample values)
static uint16_t  landed_position = 0;
static uint16_t  average = 0;
static int8_t    percentage = 0;
static const int16_t range = 993; // mean adc value at (0% altitude - 100% altitude) = 993 approximately

/*

  The handler for the ADC conversion complete interrupt.
  Writes to the circular buffer. Based on Week 4 lab ADCdemo1.c - void ADCIntHandler(void)

 */
void
adc_int_handler(void)
{
    uint32_t ulValue;

    // Get the single sample from ADC0.  ADC_BASE is defined in
    // inc/hw_memmap.h
    ADCSequenceDataGet(ADC0_BASE, 3, &ulValue);

    // Place it in the circular buffer (advancing write index)
    writeCircBuf (&g_inBuffer, ulValue);

    // Clean up, clearing the interrupt
    ADCIntClear(ADC0_BASE, 3);
}

/*

  Intializes ADC_CHANNEL9(AIN9 - PE4 on tiva board)
  Based on Week 4 lab ADCdemo1.c - void initADC (void)

*/
void
init_adc (void)
{
    // The ADC0 peripheral must be enabled for configuration and use.
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

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
    ADCIntRegister (ADC0_BASE, 3, adc_int_handler);

    // Enable interrupts for ADC0 sequence 3 (clears any outstanding interrupts)
    ADCIntEnable(ADC0_BASE, 3);

    initCircBuf (&g_inBuffer, BUF_SIZE);
}


/*

  Calculates the mean adc value when the helicopter is landed(0%). Calculated value is then used as a
  reference to calculate the altitude of the helicopter.

*/
void
calculate_landed_position(void)
{
    uint32_t sum;
    uint16_t i;
    uint16_t repeat_times;


    //3 mean values are calculated by repeating the process 3 times to get
    //the better mean value of the landed_position
    for(repeat_times = 0; repeat_times < 3 ; repeat_times++)
    {
        // Background task: calculate the (approximate) mean of the values in the
        // circular buffer
        sum = 0;

        for (i = 0; i < BUF_SIZE; i++)
        {
            sum = sum + readCircBuf (&g_inBuffer);
        }
        //Adding all the mean values
        landed_position = landed_position + (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;
    }
   // The sum of three mean values divided by 3 to get the average
    landed_position = landed_position/repeat_times;
}


void
calculate_mean_adc_and_percentage(void)
{
    uint32_t sum;
    uint16_t i;

    // Background task: calculate the (approximate) mean of the values in the
    // circular buffer
    sum = 0;

    for (i = 0; i < BUF_SIZE; i++)
    {
        sum = sum + readCircBuf (&g_inBuffer);
    }

    average = (2 * sum + BUF_SIZE) / 2 / BUF_SIZE;
    percentage = (200*(landed_position - average) + range)/(2*range);
}

int8_t
get_percentage(void)
{
    return percentage;

}

static void AdcTriggerTask(void *pvParameters)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
    UARTprintf("Adc Trigger task starting ");
    xSemaphoreGive(g_pUARTSemaphore);

    while(1)
   {
       ADCProcessorTrigger(ADC0_BASE, 3); // Initiate a conversion

       vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(10));
   }
}


//static void
//AltitudeTask(void *pvParameters)
//{
//
//    while(1)
//   {
//
//        calculate_mean_adc_and_percentage();
//
//        xSemaphoreTake(g_pUARTSemaphore, portMAX_DELAY);
//        UARTprintf("Current Altitude =  %d\n ", percentage);
//        xSemaphoreGive(g_pUARTSemaphore);
//        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(20));
//   }
//
//
//}

//uint32_t
//initAltTask(void)
//{
//    if(xTaskCreate(AltitudeTask, (const portCHAR *)"ALTITUDE", ALTITUDETASKSTACKSIZE, NULL,
//                      tskIDLE_PRIORITY + PRIORITY_ALT_TASK, NULL) != pdTRUE)
//       {
//           return(1);
//       }
//    //
//    // Success.
//    //
//    return(0);
//
//}

//uint32_t AdcTriggerTask(void)
