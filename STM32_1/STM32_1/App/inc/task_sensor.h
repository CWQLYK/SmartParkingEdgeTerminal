#ifndef __TASK_SENSOR_H
#define __TASK_SENSOR_H

#include "stm32f1xx_hal.h"
#include "main.h"
#include "HC_SR04.h"
#include "filter.h"
#include "park_state.h"

// 获取传感器数据,并对数据滤波处理，得到更稳定的距离值
void get_sensor_data_task(void);

#endif /* __TASK_SENSOR_H */

