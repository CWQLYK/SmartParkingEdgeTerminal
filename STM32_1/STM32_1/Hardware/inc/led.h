#ifndef __LED_H
#define __LED_H

#include "stm32f1xx_hal.h"

#define LED0_PIN GPIO_PIN_0
#define LED0_PORT GPIOA

#define LED1_PIN GPIO_PIN_1
#define LED1_PORT GPIOA

#define LED0_ON() HAL_GPIO_WritePin(LED0_PORT, LED0_PIN, GPIO_PIN_SET)
#define LED0_OFF() HAL_GPIO_WritePin(LED0_PORT, LED0_PIN, GPIO_PIN_RESET)
#define LED1_ON() HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_SET)
#define LED1_OFF() HAL_GPIO_WritePin(LED1_PORT, LED1_PIN, GPIO_PIN_RESET)
#define LED0_TOGGLE() HAL_GPIO_TogglePin(LED0_PORT, LED0_PIN)
#define LED1_TOGGLE() HAL_GPIO_TogglePin(LED1_PORT, LED1_PIN)

void LED_Init(void);

#endif /* __LED_H */
