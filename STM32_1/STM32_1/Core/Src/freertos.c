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
/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
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
  for (;;)
  {
    /* Sensor task code goes here */
    printf("获取超声波传感器数据...\r\n");
    double hc_sr04_distance = HC_SR04_GetDistance();
    if (hc_sr04_distance == HC_SR04_ERROR_TIMEOUT_NOSIGN)
    {
      printf("超声波传感器无响应，获取数据失败\r\n");
    }
    else if (hc_sr04_distance == HC_SR04_ERROR_TIMEOUT_NOACK)
    {
      printf("超声波传感器无确认信号，获取数据失败\r\n");
    }
    else
    {
      printf("超声波传感器数据获取成功: %.2f cm\r\n", hc_sr04_distance);
    }
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
    printf("开始判断车位状态...\r\n");
    osDelay(200);
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
    printf("低功耗模式，开始检测车位状态...\r\n");
    osDelay(1000);
  }
  /* USER CODE END Task_LowPower */
}
/* USER CODE END Application */
