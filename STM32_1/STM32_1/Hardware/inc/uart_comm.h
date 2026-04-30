#ifndef __UART_COMM_H
#define __UART_COMM_H

#include "main.h"
#include "usart.h"
#include "protocol_master.h"

// 单次接受的字节数量
#define UART2_RX_SINGLE_SIZE 1

// 发送数据超时时间
#define UART2_TX_TIMEOUT 100 // 1秒

// 串口接收缓存（环形队列，避免数据丢失）
#define UART2_RX_BUF_SIZE 128
extern uint8_t uart2_rx_buf[UART2_RX_BUF_SIZE];
extern uint16_t uart2_rx_len;

// 初始化（开启USART2接收中断）
void UART2_Comm_Init(void);

// 发送数据到ESP32
void UART2_SendData(uint8_t *data, uint16_t len);

// USART2接收中断回调（需在stm32f1xx_it.c中调用）
void UART2_RxCpltCallback(void);

void UART2_SendData_Master(uint8_t slave_addr, uint8_t state);

#endif // __UART_COMM_H

