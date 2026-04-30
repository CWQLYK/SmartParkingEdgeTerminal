#ifndef __PROTOCOL_MASTER_H
#define __PROTOCOL_MASTER_H

#include "main.h"
#include "uart_comm.h"

// 帧定义
#define FRAME_HEAD 0xAA // 帧头
#define FRAME_TAIL 0x55 // 帧尾
#define FRAME_LEN 5     // 总帧长度

// 校验位取模值
#define CHECKSUM_MOD 256

// 发送帧封装函数
uint8_t Simple_Protocol_Pack(uint8_t slave_addr, uint8_t state, uint8_t *frame_buf);

#endif
