#ifndef __USER_FREERTOS_H
#define __USER_FREERTOS_H

#include "FreeRTOS.h"
#include "task.h"

void User_Freertos_Start(void);
void Start_Task(void *pvParameters);
void cloud_task(void *pvParameters);

#endif