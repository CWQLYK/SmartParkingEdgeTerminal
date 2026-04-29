#ifndef __PARK_STATE_H
#define __PARK_STATE_H

#include "stm32f1xx_hal.h"
#include "main.h"

// ==================== 配置参数（可根据你的车位修改） ====================
#define PARK_DIST_THRESHOLD 50.0f // 车位有车阈值：≤50cm 判定有车
#define PARK_DEBOUNCE_COUNT 10    // 防抖次数：连续10次确认才切换状态

void ParkState_Init(void);
void ParkState_Update(float filter_dist);

#endif /* __PARK_STATE_H */
