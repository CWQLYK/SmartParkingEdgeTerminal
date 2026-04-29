#ifndef __TIM3_H
#define __TIM3_H

#include "stm32f1xx_hal.h"

// 外部声明定时器句柄
extern TIM_HandleTypeDef htim3;

// 函数声明
void TIM3_Init(void);
void TIM3_Start(void);
void TIM3_Stop(void);
void TIM3_ClearCnt(void);
uint32_t TIM3_GetCnt(void);

#endif

