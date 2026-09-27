/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Zero_Cross_Detect_Pin GPIO_PIN_0
#define Zero_Cross_Detect_GPIO_Port GPIOA
#define Zero_Cross_Detect_EXTI_IRQn EXTI0_IRQn
#define RELAY_FWD_Pin GPIO_PIN_2
#define RELAY_FWD_GPIO_Port GPIOA
#define Tachometer___Speed_Input_Pin GPIO_PIN_3
#define Tachometer___Speed_Input_GPIO_Port GPIOA
#define Tachometer___Speed_Input_EXTI_IRQn EXTI3_IRQn
#define RELAY_REV_Pin GPIO_PIN_4
#define RELAY_REV_GPIO_Port GPIOA
#define TRIAC_TRIG_Pin GPIO_PIN_5
#define TRIAC_TRIG_GPIO_Port GPIOA
#define TOUCH_IRQ_Pin GPIO_PIN_5
#define TOUCH_IRQ_GPIO_Port GPIOC
#define TOUCH_IRQ_EXTI_IRQn EXTI9_5_IRQn
#define TOUCH_CS_Pin GPIO_PIN_12
#define TOUCH_CS_GPIO_Port GPIOB
#define Reset_Pin GPIO_PIN_1
#define Reset_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

#endif /* __MAIN_H */
