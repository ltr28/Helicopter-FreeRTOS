//*****************************************************************************
//
// Milestone 1 - Displaying helicopter altitude as a percentage
//
// Author:  N. James
//          L. Trenberth
//          A. Chhabra
// Last modified:	31.5.2019

#include "allHeaderfiles"


//
// The mutex that protects concurrent access of UART from multiple tasks.
//
//*****************************************************************************
xSemaphoreHandle g_pUARTSemaphore;


//*****************************************************************************
//
// This hook is called by FreeRTOS when an stack overflow error is detected.
//
//*****************************************************************************
void
vApplicationStackOverflowHook(xTaskHandle *pxTask, char *pcTaskName)
{
    //
    // This function can not return, so loop forever.  Interrupts are disabled
    // on entry to this function, so no processor interrupts will interrupt
    // this loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************






//*****************************************************************************
// Main:            Controls the altitude and yaw of a model helicopter
int main(void)
{
    initADC();
    initCircBuf(bufferLocation(), BUF_SIZE);
    SysCtlDelay(SysCtlClockGet() / 12);
    resetAltitude();




    if (initAltTask() != 0){
        while(1)
        {
            //add blinking LED routine here
            //print to UART Altitude Task not working
        }
    }




    vTaskStartScheduler();

    while(1)
    {
    }
}






































//    if(initYawTask() != 0){
//        while(1)
//        {
//            //add blinking LED routine here
//            //print to UART Yaw Task not working
//        }
//    }



//
////    if(initControlTask != 0){
////        while(1)
////        {
////            //add blinking LED routine here
////            //print to UART Control Task not working
////        }
////
////    }
////
//    if(initButTask() != 0){
//        while(1)
//        {
//            //add blinking LED routine here
//            //print to UART Button Task not working
//        }
//
//    }
////
////    if(initPWMTask() != 0){
////          while(1)
////          {
////              //add blinking LED routine here
////              //print to UART PWM Task not working
////          }
////
////      }
////
//    if(initDisplayTask() != 0) {
//          while(1)
//          {
//              //add blinking LED routine here
//              //print to UART PWM Task not working
//          }
//
//      }


