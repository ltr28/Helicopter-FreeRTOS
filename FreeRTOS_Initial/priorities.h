/*****************************************************************************

 ENCE 464 Heli Rig - priorities.h

 Author:            Nathan James (44005459)
                    Luke Trenberth (47277086)
                    Abhimanyu Chhabra (99799242)

 Last modified:     21.08.2020

Purpose:            Priorities for the various FreeRTOS tasks.
                    Defines Stack Sizes for the FreeRTOS tasks
 */


#ifndef __PRIORITIES_H__
#define __PRIORITIES_H__

//*****************************************************************************
//
// The priorities of the various tasks.
//
//*****************************************************************************
#define PRIORITY_YAW_TASK           3
#define PRIORITY_BUT_TASK           3
#define PRIORITY_CONTROL_TASK       4
#define PRIORITY_DISPLAY_TASK       1
#define PRIORITY_UART_TASK          2
#define PRIORITY_ADCR_TASK          2
#define PRIORITY_ADCS_TASK          3

#define YAW_TASK_STACK_SIZE         50
#define BUT_TASK_STACK_SIZE         50
#define CONTROL_TASK_STACK_SIZE     100
#define DISPLAY_TASK_STACK_SIZE     200
#define UART_TASK_STACK_SIZE        100
#define ADCR_TASK_STACK_SIZE        100
#define ADCS_TASK_STACK_SIZE        100
#endif // __PRIORITIES_H__
