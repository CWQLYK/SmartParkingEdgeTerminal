#ifndef __USER_USART_H
#define __USER_USART_H

#include <Arduino.h>

// 调试串口：默认 Serial (UART0)，USB连接电脑
#define DEBUG_SERIAL Serial
#define DEBUG_SERIAL_BAUD 115200

// 通信串口：Serial2 (UART2)，专连 STM32
#define COMM_SERIAL Serial2
#define COMM_SERIAL_BAUD 115200

// 串口引脚定义
#define COMM_RX_PIN 16
#define COMM_TX_PIN 17

// STM32 通信帧定义（与STM32 protocol_master.h一致）
#define FRAME_HEAD 0xAA
#define FRAME_TAIL 0x55
#define FRAME_LEN  5

void uart_init_all(void);
void comm_send(uint8_t *data, uint16_t len);

#endif
