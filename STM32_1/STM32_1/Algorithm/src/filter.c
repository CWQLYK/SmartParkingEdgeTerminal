// 作者：咸
// 邮箱：3078510877@qq.com
// 创建日期：2026-4-29
// 文件说明：滤波算法实现文件

#include "filter.h"

// 滑动滤波缓存数组
static float filter_buf[FILTER_WINDOW_SIZE] = {0};
// 数组索引
static uint8_t filter_index = 0;
// 上一次有效数据
static float last_valid_data = 0;

// 初始化滤波器
void Filter_Init(void)
{
    // 清空缓存
    for (uint8_t i = 0; i < FILTER_WINDOW_SIZE; i++)
    {
        filter_buf[i] = 0;
    }

    filter_index = 0;
    last_valid_data = 0;
}

/**
 * @brief 限幅滤波 + 滑动平均滤波（组合滤波）
 * @param new_value: 原始超声波数据
 * @retval 滤波后稳定数据
 */

float Filter_GetValue(float new_value)
{
    float current_value;

    // 限幅滤波：如果新值与上次有效值差距过大，认为是异常数据，直接返回上次有效值

    if (last_valid_data == 0)
    {
        current_value = new_value; // 第一次使用，直接接受新值
    }
    else if (fabsf(new_value - last_valid_data) > MAX_DELTA)
    {
        current_value = last_valid_data; // 异常数据，返回上次有效值
    }
    else
    {
        current_value = new_value;   // 正常数据，接受新值
        last_valid_data = new_value; // 更新上次有效值
    }

    // 滑动平均滤波：将当前值加入缓存，计算平均值
    filter_buf[filter_index++] = current_value;

    if (filter_index >= FILTER_WINDOW_SIZE)
    {
        filter_index = 0; // 循环使用缓存
    }

    // 去极值滑动平均

    float max_value = 0;
    float min_value = 1000;
    float sum = 0;
    for (uint8_t i = 0; i < FILTER_WINDOW_SIZE; i++)
    {
        sum += filter_buf[i];
        if (filter_buf[i] > max_value)
        {
            max_value = filter_buf[i];
        }
        if (filter_buf[i] < min_value)
        {
            min_value = filter_buf[i];
        }
    }

    // 去极值后计算平均值
    sum -= max_value;
    sum -= min_value;

    return sum / (FILTER_WINDOW_SIZE - 2);
}
