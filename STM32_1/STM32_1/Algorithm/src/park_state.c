#include "park_state.h"

static uint8_t debounce_cnt = 0; // 防抖计数器

void ParkState_Init(void)
{
    ParkData.park_state = PARK_STATE_FREE; // 初始状态设为“空闲”
    debounce_cnt = 0;
}

/**
 * @brief  车位状态更新（核心防抖逻辑）
 * @param  filter_dist: 滤波后的超声波距离(cm)
 */
void ParkState_Update(float filter_dist)
{
    uint8_t new_state;

    // 根据距离判断当前车位状态
    if (filter_dist <= PARK_DIST_THRESHOLD)
    {
        new_state = PARK_STATE_OCCUPIED; // 距离小于等于阈值，判定为“占用”
    }
    else
    {
        new_state = PARK_STATE_FREE; // 距离大于阈值，判定为“空闲”
    }

    if(new_state != ParkData.park_state)
    {
        // 状态发生变化，进行防抖处理
        debounce_cnt++;
        if (debounce_cnt >= PARK_DEBOUNCE_COUNT)
        {
            // 连续多次确认状态变化，才真正切换状态
            ParkData.park_state = new_state;
            debounce_cnt = 0; // 重置计数器
        }
    }
    else
    {
        // 状态未发生变化，重置计数器
        debounce_cnt = 0;
    }


}
