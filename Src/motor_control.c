#include "motor_control.h"

extern TIM_HandleTypeDef htim3;

volatile uint8_t motor_speed_pct = 0;
volatile uint16_t triac_delay_us = 0;

void Motor_Control_Init(void) {
    motor_speed_pct = 0;
    triac_delay_us = 0;

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);

    HAL_TIM_Base_Stop_IT(&htim3);
}

void Motor_SetSpeed(uint8_t speed_percent) {
    if (speed_percent > 100) speed_percent = 100;
    motor_speed_pct = speed_percent;

    if (speed_percent == 0) {
        triac_delay_us = 0;
    } else {
        // Map 1-100% to timer delay (8500us down to 1000us)
        triac_delay_us = (uint16_t)(9000 - (speed_percent * 80));
    }
}

void Motor_TIM_Delay_Callback(TIM_HandleTypeDef *htim) {
    if (htim->Instance == TIM3) {
        HAL_TIM_Base_Stop_IT(&htim3);

        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
        for (volatile int i = 0; i < 2000; i++); // ~200us pulse
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);
    }
}
