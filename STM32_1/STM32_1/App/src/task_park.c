#include "task_park.h"
// 车位状态更新任务，负责根据滤波后的距离数据判断车位状态，并通过LED显示
void park_state_task(void)
{
    if (ParkData.raw_dist == HC_SR04_ERROR_TIMEOUT_NOSIGN || ParkData.raw_dist == HC_SR04_ERROR_TIMEOUT_NOACK)
    {
        return ; 
    }
    ParkState_Update(ParkData.filter_dist);
    printf("当前车位状态: %d\r\n", ParkData.park_state);

    
    // LED显示车位状态
    if (ParkData.park_state == PARK_STATE_FREE)
    {
        LED0_ON();
        LED1_OFF(); // 空闲：LED0亮
    }
    else if (ParkData.park_state == PARK_STATE_OCCUPIED)
    {
        LED0_OFF();
        LED1_ON(); // 占用：LED1亮
    }
    else
    {
        LED0_ON();
        LED1_ON(); // 未知：LED全亮
    }
}


