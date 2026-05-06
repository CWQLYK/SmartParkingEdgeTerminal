#include <Arduino.h>
#include "FreeRTOS.h"
#include "user_freertos.h"
#include "user_usart.h"
#include "user_wifi.h"

void setup() {
  // 初始化串口
  uart_init_all();
  // 连接WiFi
  wifi_init();
  // 启动FreeRTOS任务
  User_Freertos_Start();
}

void loop() {
  // 主循环可以保持空闲，所有功能通过FreeRTOS任务处理
}
