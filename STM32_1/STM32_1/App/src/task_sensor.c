#include "task_sensor.h"
#include "usart.h"

// 获取传感器数据,并对数据滤波处理，得到更稳定的距离值
void get_sensor_data_task(void)
{
    printf("获取超声波传感器数据...\r\n");
    ParkData.raw_dist = HC_SR04_GetDistance();
    if (ParkData.raw_dist == HC_SR04_ERROR_TIMEOUT_NOSIGN)
    {
        // printf("超声波传感器无响应，获取数据失败\r\n");
    }
    else if (ParkData.raw_dist == HC_SR04_ERROR_TIMEOUT_NOACK)
    {
        // printf("超声波传感器无确认信号，获取数据失败\r\n");
    }
    else
    {
        // 对获取到的原始距离数据进行滤波处理，得到更稳定的距离值
        ParkData.filter_dist = Filter_GetValue(ParkData.raw_dist);
        if (Filter_Distances_Index >= 10) // 环形缓冲区索引，指向最新的滤波数据位置
        {
            Filter_Distances_Index = 0;
        }
        Filter_Distances[Filter_Distances_Index++] = ParkData.filter_dist; // 存储滤波后的距离数据
        printf("原始距离: %.2f cm, 滤波后距离: %.2f cm\r\n", ParkData.raw_dist, ParkData.filter_dist);
    }
}




