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

#define MS 50

typedef enum {
    MALE,
    FEMALE,
    NONBINARY
} sex;

typedef struct User{
    char* m_name;
    int16_t m_age;
    sex m_sex;
} User;

QueueHandle_t myQueue1;
QueueHandle_t myQueue2;

QueueSetHandle_t myQueueSet;

void queueReceiveTask(void *params);
void queueSendTask1(void *params);
void queueSendTask2(void *params);

void main_mine( void ) {
    BaseType_t status;

    User me = {"Ethan", 21, MALE };
    User mom = {"Kira", 46, FEMALE };
    User dad = {"Jason", 52, MALE };
    User Izz = {"Izz", 20, NONBINARY};

    myQueue1 = xQueueCreate(5, sizeof(User));
    myQueue2 = xQueueCreate(5, sizeof(User));
    if (myQueue1 == NULL)
        printf("Memory could not be allocated for queue 1\n");

    if (myQueue2 == NULL)
        printf("Memory could not be allocated for queue 2\n");

    myQueueSet = xQueueCreateSet(10);
    if (myQueueSet == NULL)
        printf("Queue set was not created successfully\n");
    
    status = xQueueAddToSet(myQueue1, myQueueSet);
    if(status == pdFAIL)
        printf("Error adding queue 1 to set\n");
    
    status = xQueueAddToSet(myQueue2, myQueueSet);
    if(status == pdFAIL)
        printf("Error adding queue 2 to set\n");


    xTaskCreate(queueSendTask1, "Send 1", 1000, &me, 1, NULL);
    xTaskCreate(queueSendTask2, "Send 2", 1000, &mom, 1, NULL);

    xTaskCreate(queueReceiveTask, "Receive", 1000, NULL, 1, NULL);
    vTaskStartScheduler();

    for(;;);
}

void queueSendTask1 (void *params){
    User *currentUser = (User*) params;
    TickType_t ticksToWait = pdMS_TO_TICKS(MS);
    //printf("queueSendTask received the following string: %s\n", s);

    BaseType_t status;

    for (;;){
        vTaskDelay(ticksToWait);

        status = xQueueSend(myQueue1, &currentUser, 0);
        //vTaskDelay(pdMS_TO_TICKS(100));

        if (status != pdPASS)
            printf("Not able to send to queue\n");
    }
}

void queueSendTask2 (void *params){
    User *currentUser = (User*) params;
    TickType_t ticksToWait = pdMS_TO_TICKS(MS);
    //printf("queueSendTask received the following string: %s\n", s);

    BaseType_t status;

    for (;;){
        vTaskDelay(ticksToWait * 4);

        status = xQueueSend(myQueue2, &currentUser, 0);
        //vTaskDelay(pdMS_TO_TICKS(100));

        if (status != pdPASS)
            printf("Not able to send to queue\n");
    }
}

void queueReceiveTask(void *params){
    User* currentUser;
    BaseType_t status;
    char* userSex;
    QueueHandle_t currentQueue;
    TickType_t ticksToWait = pdMS_TO_TICKS(MS);

    for(;;){
        currentQueue = (QueueHandle_t) xQueueSelectFromSet(myQueueSet, 0);
        if (currentQueue == NULL){
            //printf("Could not select queue\n"); 
            continue;
        }
        status = xQueueReceive(currentQueue, &currentUser, 0);

        if (status != pdPASS)
            printf("Unable to pull from queue");
        else{
            switch (currentUser->m_sex){
                case MALE:
                    userSex = "Male";
                    break;
                case FEMALE:
                    userSex = "Female";
                    break;
                case NONBINARY:
                    userSex = "Non-binary";
                    break;
            }

            printf("%s is %d years old and %s\n",currentUser->m_name,currentUser->m_age,userSex);
        }

        //vPortFree(buffer);
    }
}