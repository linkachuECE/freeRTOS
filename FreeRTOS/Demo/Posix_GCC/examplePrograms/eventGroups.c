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

EventBits_t myEvents;

#define FIRSTTASK_BIT (1UL << 0UL)
#define SECONDTASK_BIT (1UL << 1UL)

void task1(void* param);
void task2(void* param);
void inputTask(void* param);

EventGroupHandle_t eventHandler;

void main_mine( void ) {

    eventHandler = xEventGroupCreate();

    xEventGroupSetBits(eventHandler, 0);

    if (xTaskCreate(task1, "Task 1", 1000, NULL, 2, NULL) != pdPASS){
        printf("task 1 could not be created\n");
        exit(EXIT_FAILURE);
    }
    if (xTaskCreate(task2, "Task 2", 1000, NULL, 2, NULL) != pdPASS){
        printf("task 2 could not be created\n");
        exit(EXIT_FAILURE);
    }

    if (xTaskCreate(inputTask, "Input 2", 1000, NULL, 1, NULL) != pdPASS){
        printf("Input task could not be created\n");
        exit(EXIT_FAILURE);
    }

    vTaskStartScheduler();
}

void task1(void* param){
    for(;;){
        xEventGroupWaitBits(eventHandler, FIRSTTASK_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
        printf("Task 1 activated\n");
    }
}

void task2(void* param){
    EventBits_t eventGroupValue;
    for(;;){
        xEventGroupWaitBits(eventHandler, SECONDTASK_BIT, pdTRUE, pdFALSE, portMAX_DELAY);
        printf("Task 2 activated\n");
    }
}

void inputTask(void* param){
    EventBits_t input = -1;
    for(;;){
        vTaskDelay(pdMS_TO_TICKS(1000));
        if(input == -1 || input == SECONDTASK_BIT)
            input = FIRSTTASK_BIT;
        else
            input = SECONDTASK_BIT;
        
        xEventGroupSetBits(eventHandler, input);
    }
}
