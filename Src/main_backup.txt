/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Smart Drill Phase Control with RPM Tachometer (PA3)
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "gpio.h"
#include "fsmc.h"

/* USER CODE BEGIN Includes */
#include "ili9341.h"
#include "xpt2046.h"
#include "motor_control.h"

#ifndef COLOR_YELLOW
#define COLOR_YELLOW 0xFFE0
#endif
#ifndef COLOR_MAGENTA
#define COLOR_MAGENTA 0xF81F
#endif
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
typedef enum {
    DIR_STOP = 0,
    DIR_RIGHT,
    DIR_LEFT
} Motor_Direction_t;

/* Private variables ---------------------------------------------------------*/
static volatile Motor_Direction_t current_dir = DIR_STOP;
static volatile uint8_t motor_speed = 50;

/* Tachometer Variables */
static volatile uint32_t pulse_count = 0;
static uint32_t current_rpm = 0;
static uint32_t last_rpm_tick = 0;

#define PULSES_PER_REV 1 // Set according to your encoder disc slots

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN 0 */
void Redraw_Speed_Display(void)
{
    char speed_str[16];
    ILI9341_FillRectangle(90, 140, 60, 30, COLOR_BLACK);

    speed_str[0] = (motor_speed / 100) + '0';
    speed_str[1] = ((motor_speed % 100) / 10) + '0';
    speed_str[2] = (motor_speed % 10) + '0';
    speed_str[3] = '%';
    speed_str[4] = '\0';

    if (motor_speed < 100)
    {
        ILI9341_DrawString(95, 145, &speed_str[1], COLOR_YELLOW, COLOR_BLACK);
    }
    else
    {
        ILI9341_DrawString(90, 145, speed_str, COLOR_YELLOW, COLOR_BLACK);
    }
}

void Redraw_RPM_Display(void)
{
    char rpm_str[20];
    ILI9341_FillRectangle(70, 290, 100, 25, COLOR_BLACK);

    /* Convert RPM number to String */
    rpm_str[0] = 'R'; rpm_str[1] = 'P'; rpm_str[2] = 'M'; rpm_str[3] = ':'; rpm_str[4] = ' ';
    rpm_str[5] = (current_rpm / 1000) + '0';
    rpm_str[6] = ((current_rpm % 1000) / 100) + '0';
    rpm_str[7] = ((current_rpm % 100) / 10) + '0';
    rpm_str[8] = (current_rpm % 10) + '0';
    rpm_str[9] = '\0';

    ILI9341_DrawString(75, 295, rpm_str, COLOR_WHITE, COLOR_BLACK);
}

void Draw_UI(void)
{
    ILI9341_FillRectangle(0, 0, 240, 320, COLOR_BLACK);

    /* Left Arrow Button */
    if (current_dir == DIR_LEFT) {
        ILI9341_FillRectangle(15, 30, 90, 70, COLOR_GREEN);
        ILI9341_DrawString(50, 55, "<-", COLOR_WHITE, COLOR_GREEN);
    } else {
        ILI9341_FillRectangle(15, 30, 90, 70, COLOR_BLUE);
        ILI9341_DrawString(50, 55, "<-", COLOR_WHITE, COLOR_BLUE);
    }

    /* Right Arrow Button */
    if (current_dir == DIR_RIGHT) {
        ILI9341_FillRectangle(135, 30, 90, 70, COLOR_GREEN);
        ILI9341_DrawString(170, 55, "->", COLOR_WHITE, COLOR_GREEN);
    } else {
        ILI9341_FillRectangle(135, 30, 90, 70, COLOR_BLUE);
        ILI9341_DrawString(170, 55, "->", COLOR_WHITE, COLOR_BLUE);
    }

    /* Minus Button */
    ILI9341_FillRectangle(15, 130, 60, 50, COLOR_MAGENTA);
    ILI9341_DrawString(40, 145, "-", COLOR_WHITE, COLOR_MAGENTA);

    /* Plus Button */
    ILI9341_FillRectangle(165, 130, 60, 50, COLOR_MAGENTA);
    ILI9341_DrawString(190, 145, "+", COLOR_WHITE, COLOR_MAGENTA);

    Redraw_Speed_Display();

    /* Stop Button */
    ILI9341_FillRectangle(35, 200, 170, 50, COLOR_RED);
    ILI9341_DrawString(100, 215, "STOP", COLOR_WHITE, COLOR_RED);

    Redraw_RPM_Display();
}

/* EXTI Callbacks for Zero-Cross (PA0) and Tachometer (PA3) */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    /* Zero-Cross Interrupt */
    if (GPIO_Pin == GPIO_PIN_0)
    {
        if (current_dir == DIR_STOP || motor_speed == 0)
        {
            GPIOA->BSRR = GPIO_PIN_5; /* Force PA5 HIGH */
            return;
        }

        uint32_t delay_loops = 300 + ((uint32_t)(100 - motor_speed) * 648);
        for (volatile uint32_t i = 0; i < delay_loops; i++);

        GPIOA->BSRR = (uint32_t)GPIO_PIN_5 << 16U; /* PA5 LOW */
        for (volatile uint16_t p = 0; p < 600; p++);
        GPIOA->BSRR = GPIO_PIN_5;                  /* PA5 HIGH */
    }
    /* Tachometer Speed Pulse Interrupt on PA3 */
    else if (GPIO_Pin == GPIO_PIN_3)
    {
        pulse_count++;
    }
}
/* USER CODE END 0 */

int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_FSMC_Init();
  MX_SPI2_Init();

  /* USER CODE BEGIN 2 */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* PA5 Setup */
  GPIOA->BSRR = GPIO_PIN_5;
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = GPIO_PIN_5;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

  /* PA0 EXTI Zero-Cross Input Setup */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* PA3 EXTI Tachometer Speed Input Setup */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* Relays Setup (PA2, PA4) */
  GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

  /* Set NVIC Interrupt Priorities */
  HAL_NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);
  HAL_NVIC_SetPriority(EXTI0_IRQn, 4, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  ILI9341_Init();
  Draw_UI();

  Touch_Point t_point;
  uint8_t touch_lock = 0;
  last_rpm_tick = HAL_GetTick();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      /* 1. Calculate and Refresh RPM every 1000ms */
      if ((HAL_GetTick() - last_rpm_tick) >= 1000)
      {
          last_rpm_tick = HAL_GetTick();

          /* Calculate RPM */
          current_rpm = (pulse_count * 60) / PULSES_PER_REV;
          pulse_count = 0; // Reset counter for next second

          /* Update Screen */
          Redraw_RPM_Display();
      }

      /* 2. Touch Screen Handling */
      if (XPT2046_Get_Touch_Calibrated(&t_point))
      {
          if (!touch_lock)
          {
              touch_lock = 1;

              /* Stop Button */
              if (t_point.y >= 190 && t_point.y <= 250 && t_point.x >= 35 && t_point.x <= 205)
              {
                  current_dir = DIR_STOP;
                  GPIOA->BSRR = GPIO_PIN_5;
                  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);
                  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
                  Draw_UI();
              }
              /* Speed Control Area */
              else if (t_point.y >= 110 && t_point.y <= 180)
              {
                  if (t_point.x >= 10 && t_point.x <= 85)
                  {
                      if (motor_speed >= 10) motor_speed -= 10;
                      Redraw_Speed_Display();
                  }
                  else if (t_point.x >= 155 && t_point.x <= 230)
                  {
                      if (motor_speed <= 90) motor_speed += 10;
                      Redraw_Speed_Display();
                  }
              }
              /* Direction Control Area */
              else if (t_point.y >= 30 && t_point.y <= 100)
              {
                  if (t_point.x >= 15 && t_point.x <= 105)
                  {
                      if (current_dir == DIR_STOP)
                      {
                          current_dir = DIR_LEFT;
                          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
                          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);

                          ILI9341_FillRectangle(15, 30, 90, 70, COLOR_GREEN);
                          ILI9341_DrawString(50, 55, "<-", COLOR_WHITE, COLOR_GREEN);
                          ILI9341_FillRectangle(135, 30, 90, 70, COLOR_BLUE);
                          ILI9341_DrawString(170, 55, "->", COLOR_WHITE, COLOR_BLUE);
                      }
                  }
                  else if (t_point.x >= 135 && t_point.x <= 225)
                  {
                      if (current_dir == DIR_STOP)
                      {
                          current_dir = DIR_RIGHT;
                          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
                          HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

                          ILI9341_FillRectangle(135, 30, 90, 70, COLOR_GREEN);
                          ILI9341_DrawString(170, 55, "->", COLOR_WHITE, COLOR_GREEN);
                          ILI9341_FillRectangle(15, 30, 90, 70, COLOR_BLUE);
                          ILI9341_DrawString(50, 55, "<-", COLOR_WHITE, COLOR_BLUE);
                      }
                  }
              }
          }
      }
      else
      {
          touch_lock = 0;
      }

      HAL_Delay(20);
  }
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5);
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}
