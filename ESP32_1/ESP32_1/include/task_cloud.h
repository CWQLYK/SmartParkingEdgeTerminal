#ifndef __TASK_CLOUD_H
#define __TASK_CLOUD_H

#include "user_usart.h"

// MQTT Broker 地址（修改为你电脑的IP地址）
#define MQTT_BROKER_IP "broker.emqx.io"
#define MQTT_BROKER_PORT 1883
#define MQTT_TOPIC "park/status"

// 全局变量：最新的车位状态（由串口任务写入，云任务读取后清除）
extern volatile uint8_t g_park_state;
extern volatile bool g_park_state_updated;

void cloud_upload_task(void *pvParameters);

#endif
