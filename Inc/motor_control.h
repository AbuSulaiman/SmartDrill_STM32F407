#ifndef __MOTOR_CONTROL_H
#define __MOTOR_CONTROL_H

#include "main.h"

void Motor_Control_Init(void);
void Motor_SetSpeed(uint8_t speed_percent);
void Motor_TIM_Delay_Callback(TIM_HandleTypeDef *htim); // Declaration needed for stm32f4xx_it.c

#endif /* __MOTOR_CONTROL_H */
