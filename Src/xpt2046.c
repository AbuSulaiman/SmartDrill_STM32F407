#include "xpt2046.h"

extern SPI_HandleTypeDef hspi2;

#define CMD_READ_X 0x90
#define CMD_READ_Y 0xD0

#define TOUCH_MIN_X 240
#define TOUCH_MAX_X 3800
#define TOUCH_MIN_Y 240
#define TOUCH_MAX_Y 3800

#define SAMPLES_COUNT 8

static uint16_t XPT2046_Read_ADC(uint8_t cmd) {
    uint8_t tx_data[3] = {cmd, 0x00, 0x00};
    uint8_t rx_data[3] = {0};

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(&hspi2, tx_data, rx_data, 3, 10);
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

    return ((rx_data[1] << 8) | rx_data[2]) >> 3;
}

// Average Filter to completely eliminate jitter/scatter
static void XPT2046_Read_Filtered(uint16_t *out_x, uint16_t *out_y) {
    uint32_t sum_x = 0;
    uint32_t sum_y = 0;

    for (uint8_t i = 0; i < SAMPLES_COUNT; i++) {
        sum_x += XPT2046_Read_ADC(CMD_READ_X);
        sum_y += XPT2046_Read_ADC(CMD_READ_Y);
    }

    *out_x = (uint16_t)(sum_x / SAMPLES_COUNT);
    *out_y = (uint16_t)(sum_y / SAMPLES_COUNT);
}

uint8_t XPT2046_IsTouched(void) {
    return (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_5) == GPIO_PIN_RESET);
}

uint8_t XPT2046_Get_Touch_Calibrated(Touch_Point *p) {
    if (!XPT2046_IsTouched()) {
        p->touched = 0;
        return 0;
    }

    uint16_t raw_adc_x = 0;
    uint16_t raw_adc_y = 0;

    // Read averaged ADC samples
    XPT2046_Read_Filtered(&raw_adc_x, &raw_adc_y);

    if (raw_adc_x < TOUCH_MIN_X) raw_adc_x = TOUCH_MIN_X;
    if (raw_adc_x > TOUCH_MAX_X) raw_adc_x = TOUCH_MAX_X;
    if (raw_adc_y < TOUCH_MIN_Y) raw_adc_y = TOUCH_MIN_Y;
    if (raw_adc_y > TOUCH_MAX_Y) raw_adc_y = TOUCH_MAX_Y;

    // Direct mapping with filtered precision
    uint16_t mapped_x = (uint16_t)(((uint32_t)(TOUCH_MAX_Y - raw_adc_y) * 240) / (TOUCH_MAX_Y - TOUCH_MIN_Y));
    uint16_t mapped_y = (uint16_t)(((uint32_t)(raw_adc_x - TOUCH_MIN_X) * 320) / (TOUCH_MAX_X - TOUCH_MIN_X));

    p->x = mapped_x;
    p->y = mapped_y;
    p->touched = 1;

    return 1;
}
