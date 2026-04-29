#include "led.h"
#include "usart.h"
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable the GPIO clock for the LED pins */
    __HAL_RCC_GPIOA_CLK_ENABLE();

    /* Configure LED0 pin */
    GPIO_InitStruct.Pin = LED0_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED0_PORT, &GPIO_InitStruct);

    /* Configure LED1 pin */
    GPIO_InitStruct.Pin = LED1_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LED1_PORT, &GPIO_InitStruct);

    LED0_OFF();
    LED1_OFF();

    printf("LEDs initialized.\r\n");
}
