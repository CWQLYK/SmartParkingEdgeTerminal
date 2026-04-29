#include "tim3.h"
#include "usart.h"
TIM_HandleTypeDef htim3;

void TIM3_Init(void)
{
    // 开启TIM3时钟
    __HAL_RCC_TIM3_CLK_ENABLE();

    // 配置TIM3
    // 定时器更新周期（秒）= (PSC + 1) * (ARR + 1) / 定时器输入时钟频率
    // 计数器时钟频率 = 定时器输入时钟频率 / (预分频系数 PSC + 1)
    htim3.Instance = TIM3;
    htim3.Init.Prescaler = 72 - 1; // 72
    htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim3.Init.Period = 65535 - 1; // 10ms
    htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;

    if (HAL_TIM_Base_Init(&htim3) == HAL_ERROR)
    {
        printf("TIM3 Init Error!!\r\n");
        Error_Handler();
    }

    TIM3_Stop();
    TIM3_ClearCnt();
    printf("TIM3 initialized.\r\n");
}

// 启动TIM3
void TIM3_Start(void)
{
    TIM3_ClearCnt();
    HAL_TIM_Base_Start(&htim3);
}

// 停止TIM3
void TIM3_Stop(void)
{
    HAL_TIM_Base_Stop(&htim3);
    TIM3_ClearCnt();
}

// 清除TIM3计数器
void TIM3_ClearCnt(void)
{
    __HAL_TIM_SET_COUNTER(&htim3, 0);
}

// 获取TIM3计数器的当前值
uint32_t TIM3_GetCnt(void)
{
    return __HAL_TIM_GET_COUNTER(&htim3);
}
