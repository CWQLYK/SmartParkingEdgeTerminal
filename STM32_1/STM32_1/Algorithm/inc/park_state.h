#ifndef __PARK_STATE_H
#define __PARK_STATE_H

#include "stm32f1xx_hal.h"
#include "main.h"

// 阈值配置（AI 置信度不足时的兜底判定）
#define PARK_DIST_THRESHOLD 50.0f
#define PARK_DEBOUNCE_COUNT 10

void ParkState_Init(void);
void ParkState_Update(float filter_dist);

#endif
