#include "user_freertos.h"
#include "FreeRTOS.h"
#include "task.h"
#include "user_usart.h"
#include "task_usart.h"
#include "task_cloud.h"

#define STATRO_TASK_PRIORITY 1
#define STATRO_TASK_STACK_SIZE 2048
TaskHandle_t start_task_handle;

#define COMM_TASK_PRIORITY  1
#define COMM_TASK_STACK_SIZE 2048
TaskHandle_t comm_task_handle;

#define CLOUD_TASK_PRIORITY  2
#define CLOUD_TASK_STACK_SIZE 4096
TaskHandle_t cloud_task_handle;

void comm_task(void *pvParameters);

void User_Freertos_Start(void)
{
    xTaskCreate(
        Start_Task,
        "Start_Task",
        STATRO_TASK_STACK_SIZE,
        NULL,
        STATRO_TASK_PRIORITY,
        &start_task_handle);
}

void Start_Task(void *pvParameters)
{
    xTaskCreate(
        comm_task,
        "comm_task",
        COMM_TASK_STACK_SIZE,
        NULL,
        COMM_TASK_PRIORITY,
        &comm_task_handle);

    xTaskCreate(
        cloud_task,
        "cloud_task",
        CLOUD_TASK_STACK_SIZE,
        NULL,
        CLOUD_TASK_PRIORITY,
        &cloud_task_handle);

    vTaskDelete(NULL);
}

void comm_task(void *pvParameters)
{
    while (1)
    {
        user_usart_task();
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void cloud_task(void *pvParameters)
{
    cloud_upload_task(pvParameters);
}
