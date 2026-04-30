/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2026 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "led.h"
#include "usart.h"
#include "HC_SR04.h"
#include "filter.h"
#include "park_state.h"
#include "ai_parking.h"
#include "task_sensor.h"
#include "task_park.h"
#include "task_uart.h"
// #include "FreeRTOSConfig.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
osThreadId_t sensorTaskHandle;
const osThreadAttr_t sensorTask_attributes = {
    .name = "sensorTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

osThreadId_t parkTaskHandle;
const osThreadAttr_t parkTask_attributes = {
    .name = "parkTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityBelowNormal7,
};

osThreadId_t UARTTaskHandle;
const osThreadAttr_t UARTTask_attributes = {
    .name = "UARTTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityBelowNormal6,
};

osThreadId_t lowPowerTaskHandle;
const osThreadAttr_t lowPowerTask_attributes = {
    .name = "lowPowerTask",
    .stack_size = 128 * 4,
    .priority = (osPriority_t)osPriorityBelowNormal5,
};

void Task_Sensor(void *argument);
void Task_Park(void *argument);
void Task_UART(void *argument);
void Task_LowPower(void *argument);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  printf("Default task is running...\r\n");
  /* Infinite loop */
  /*进入临界区*/
  taskENTER_CRITICAL();
  /*创建任务*/
  sensorTaskHandle = osThreadNew(Task_Sensor, NULL, &sensorTask_attributes);
  parkTaskHandle = osThreadNew(Task_Park, NULL, &parkTask_attributes);
  UARTTaskHandle = osThreadNew(Task_UART, NULL, &UARTTask_attributes);
  lowPowerTaskHandle = osThreadNew(Task_LowPower, NULL, &lowPowerTask_attributes);

  printf("All tasks created.\r\n");
  /*退出临界区*/
  taskEXIT_CRITICAL();

  /*删除自身任务*/
  vTaskDelete(NULL);
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void Task_Sensor(void *argument)
{
  /* USER CODE BEGIN Task_Sensor */

  ParkData.raw_dist = 0;
  ParkData.filter_dist = 0;

  for (;;)
  {
    /* Sensor task code goes here */
    get_sensor_data_task();

    osDelay(100);
  }
  /* USER CODE END Task_Sensor */
}

void Task_Park(void *argument)
{
  /* USER CODE BEGIN Task_Park */
  for (;;)
  {
    /* Park task code goes here */
    printf("检测车位状态...\r\n");
    park_state_task();
    osDelay(100);
  }
  /* USER CODE END Task_Park */
}

void Task_UART(void *argument)
{
  /* USER CODE BEGIN Task_UART */
  for (;;)
  {
    /* UART task code goes here */
    printf("开始传输数据给ESP32...\r\n");
    send_parking_data_task(); // 发送当前车位状态
    osDelay(500);
  }
  /* USER CODE END Task_UART */
}

void Task_LowPower(void *argument)
{
  /* USER CODE BEGIN Task_LowPower */
  for (;;)
  {
    /* Low power task code goes here */
    // printf("低功耗模式，开始检测车位状态...\r\n");
    osDelay(1000);
  }
  /* USER CODE END Task_LowPower */
}
/* USER CODE END Application */
