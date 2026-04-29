#ifndef __FILTER_H
#define __FILTER_H

#include "stm32f1xx_hal.h"

// 滤波配置参数（可根据你的需求修改）
#define FILTER_WINDOW_SIZE 10 // 滑动窗口：取最近10次数据平均
#define MAX_DELTA 50          // 限幅阈值：单次距离变化超过50cm，判定为异常值

// 函数声明
float Filter_GetValue(float new_value); // 组合滤波主函数
void Filter_Init(void);                 // 滤波初始化

#endif
