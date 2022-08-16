#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

/* Kernel includes. */
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "timers.h"
#include "semphr.h"
#include "event_groups.h"

/* Local includes. */
#include "console.h"

SemaphoreHandle_t myMutex;

void task1(void *param);
void task2(void *param);

void main_mine( void ) {
    myMutex = xSemaphoreCreateRecursiveMutex();
    
    xTaskCreate(task1, "First task", 1000, NULL, 2, NULL);
    xTaskCreate(task2, "Second task", 1000, NULL, 2, NULL);

    vTaskStartScheduler();
}

void task1(void *param){
    for(;;){
        if (xSemaphoreTakeRecursive(myMutex, portMAX_DELAY) == pdPASS)
            printf("Task 1: Mutex taken\n");
        else
            printf("Task 1: Failed to take Mutex");
        
        printf("Task 1: Entered\n");
        printf("Delaying for 1 second\n");

        vTaskDelay((rand() % pdMS_TO_TICKS(500)) + pdMS_TO_TICKS(500));

        if (xSemaphoreGiveRecursive(myMutex) == pdPASS)
            printf("Task 1: Mutex given back\n");
        else
            printf("Task 1: Failed to return Mutex");

    }
}

void task2(void *param){
    for(;;){
        if (xSemaphoreTakeRecursive(myMutex, portMAX_DELAY) == pdPASS)
            printf("Task 2: Mutex taken\n");
        else
            printf("Task 2: Failed to take Mutex");
        
        printf("Task 2: Entered\n");
        vTaskDelay((rand() % pdMS_TO_TICKS(500)) + pdMS_TO_TICKS(500));

        if (xSemaphoreGiveRecursive(myMutex) == pdPASS)
            printf("Task 2: Mutex given back\n");
        else
            printf("Task 2: Failed to return Mutex");

    }
}