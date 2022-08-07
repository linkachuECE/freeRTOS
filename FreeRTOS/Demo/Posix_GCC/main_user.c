#include <stdio.h>
#include <pthread.h>
#include <string.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"

/* Local includes. */
#include "console.h"

QueueHandle_t myQueue;
void queueReceiveTask(void *params);
void queueSendTask(void *params);

void main_mine( void ) {

    myQueue = xQueueCreate(2, sizeof(int32_t));

    if (myQueue == NULL)
        printf("Memory could not be allocated for queue");

    xTaskCreate(queueSendTask, "Send 1", 1000, "The monkey", 1, NULL);
    xTaskCreate(queueSendTask, "Send 2", 1000, "The chicken", 1, NULL);

    xTaskCreate(queueReceiveTask, "Receive", 1000, NULL, 2, NULL);
    vTaskStartScheduler();

    for(;;);
}

void queueSendTask (void *params){
    char* s = (char*)pvPortMalloc(32*sizeof(char));
    strcpy(s, (char*)params);
    //printf("queueSendTask received the following string: %s\n", s);

    BaseType_t status;

    for (;;){
        status = xQueueSend(myQueue, &s, 0);
        //vTaskDelay(pdMS_TO_TICKS(100));

        if (status != pdPASS)
            printf("Not able to send to queue\n");
    }
}

void queueReceiveTask(void *params){
    char* buffer;
    BaseType_t status;
    TickType_t ticksToWait = pdMS_TO_TICKS(100);

    for(;;){
        if (uxQueueMessagesWaiting(myQueue) != 0)
            printf("Queue should have been empty\n");
        
        status = xQueueReceive(myQueue, &buffer, ticksToWait);

        if (status != pdPASS)
            printf("Unable to pull from queue");
        else
            printf("Received string: %s\n", buffer);

        //vPortFree(buffer);
    }
}