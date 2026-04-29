#include "HC_SR04.h"

void HC_SR04_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable the GPIO clock for the HC-SR04 pins */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure TRIG pin as output */
    GPIO_InitStruct.Pin = TRIG_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(TRIG_PORT, &GPIO_InitStruct);

    /* Configure ECHO pin as input */
    GPIO_InitStruct.Pin = ECHO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(ECHO_PORT, &GPIO_InitStruct);

    /* Set TRIG pin to low initially */
    HAL_GPIO_WritePin(TRIG_PORT, TRIG_PIN, GPIO_PIN_RESET);

    printf("HC-SR04 initialized.\r\n");
}

// 获取距离，单位为厘米，返回-1表示测距失败
double HC_SR04_GetDistance(void)
{
    uint32_t time_us = 0;

    TIM3_Start();

    // 发送trig信号
    TRIG_UP();
    while (TIM3_GetCnt() < TRIG_PULSE_TIME)
        ;
    TRIG_DOWN();
    TIM3_ClearCnt();

    int timeout_counter = 0;
    // 等待echo信号变高
    while (ECHO_READ() == GPIO_PIN_RESET)
    {
        timeout_counter++;
        if (timeout_counter > 1e7) // 超过一定次数仍未收到echo信号，认为测距失败
        {
            // 超时处理
            TIM3_Stop();
            return HC_SR04_ERROR_TIMEOUT_NOSIGN;
        }
    }

    // 计时开始
    timeout_counter = 0;
    TIM3_ClearCnt();

    while (ECHO_READ() == GPIO_PIN_SET)
    {
        timeout_counter++;
        if (timeout_counter > 1e5) // 超过一定次数仍未收到echo信号，认为测距失败
        {
            // 超时处理
            TIM3_Stop();
            return HC_SR04_ERROR_TIMEOUT_NOACK;
        }
    }

    // 读取时间
    time_us = TIM3_GetCnt();
    TIM3_Stop();

    // 计算距离
    double distance = (double)(time_us * SPEED_OF_SOUND_CM_PER_US_HALF); // 距离 = 时间 * 声速 / 2

    return distance;
}