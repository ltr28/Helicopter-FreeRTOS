/*

 *
 *  Created on: 5/08/2020
 *      Author: nja100
 */

#ifndef HELIQUEUE_H_
#define HELIQUEUE_H_


void
vReceiverTask(void *pvParameters);

void
vSenderTask(void *pvParameters );

void
initQueue(void);

#endif /* HELIQUEUE_H_ */
