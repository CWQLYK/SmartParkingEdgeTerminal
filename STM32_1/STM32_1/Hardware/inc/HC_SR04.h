#ifndef __HC_SR04_H
#define __HC_SR04_H

#include "stm32f1xx_hal.h"
#include "usart.h"
#include "tim3.h"
#define TRIG_PIN GPIO_PIN_7
#define TRIG_PORT GPIOA
#define ECHO_PIN GPIO_PIN_6
#define ECHO_PORT GPIOA

#define SPEED_OF_SOUND 343.0                  // 声速，单位为米/秒
#define SPEED_OF_SOUND_CM_PER_US 0.0343       // 声速，单位为厘米/微秒
#define SPEED_OF_SOUND_CM_PER_US_HALF 0.01715 // 声速的一半，单位为厘米/微秒
#define TRIG_PULSE_TIME 15                    // TRIG信号持续时间，单位为微秒
#define HC_SR04_TIMEOUT 60000                 // 超时时间，单位为微秒
// ECHO一直未收到信号时，认为测距失败,无响应，返回-1
#define HC_SR04_ERROR_TIMEOUT_NOSIGN -1.0
// ECHO信号持续时间过长时，认为测距失败，返回-2
#define HC_SR04_ERROR_TIMEOUT_NOACK -2.0

#define TRIG_UP() HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_SET)
#define TRIG_DOWN() HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET)
#define ECHO_READ() HAL_GPIO_ReadPin(ECHO_PORT, ECHO_PIN)

void HC_SR04_Init(void);
float HC_SR04_GetDistance(void);
#endif
