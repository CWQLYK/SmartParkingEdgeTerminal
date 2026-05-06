#ifndef __AI_PARKING_H
#define __AI_PARKING_H

#include "stm32f1xx_hal.h"
#include <math.h>

// AI 预测结果
#define AI_CONFIDENCE_HIGH   0.9f
#define AI_CONFIDENCE_MEDIUM 0.7f
#define AI_CONFIDENCE_LOW    0.5f

// 归一化因子：输入距离(cm) / NORM_SCALE，将距离映射到合理范围
// HC-SR04 典型量程 2~400cm，除以 100 后输入范围 ~0.02~4.0
#define NORM_SCALE 1.0f

/**
 * @brief 使用边缘 AI 模型预测车位占用概率
 * @param features 输入特征数组（10个连续滤波距离值，单位cm）
 * @return 0.0~1.0，>0.5 表示占用，<0.5 表示空闲
 */
float ai_parking_predict(float *features);

/**
 * @brief 初始化 AI 推理模块
 */
void AI_Parking_Init(void);

#endif
