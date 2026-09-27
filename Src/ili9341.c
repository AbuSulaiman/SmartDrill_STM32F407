#include "ili9341.h"
#include "lcd_font.h"

static void ILI9341_Write_Cmd(uint16_t cmd) {
    LCD_REG = cmd;
}

static void ILI9341_Write_Data(uint16_t data) {
    LCD_DATA = data;
}

static void ILI9341_Set_Address_Window(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    ILI9341_Write_Cmd(0x2A);
    ILI9341_Write_Data(x1 >> 8);
    ILI9341_Write_Data(x1 & 0xFF);
    ILI9341_Write_Data(x2 >> 8);
    ILI9341_Write_Data(x2 & 0xFF);

    ILI9341_Write_Cmd(0x2B);
    ILI9341_Write_Data(y1 >> 8);
    ILI9341_Write_Data(y1 & 0xFF);
    ILI9341_Write_Data(y2 >> 8);
    ILI9341_Write_Data(y2 & 0xFF);

    ILI9341_Write_Cmd(0x2C);
}

void ILI9341_Init(void) {
    ILI9341_Write_Cmd(0x01); // Reset
    HAL_Delay(100);

    ILI9341_Write_Cmd(0x36);
    ILI9341_Write_Data(0x48); // Correct Orientation & Mirroring Fix

    ILI9341_Write_Cmd(0x3A);
    ILI9341_Write_Data(0x55);

    ILI9341_Write_Cmd(0x11);
    HAL_Delay(120);

    ILI9341_Write_Cmd(0x29);
}

void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color) {
    if ((x >= 240) || (y >= 320)) return;
    if ((x + width - 1) >= 240) width = 240 - x;
    if ((y + height - 1) >= 320) height = 320 - y;

    ILI9341_Set_Address_Window(x, y, x + width - 1, y + height - 1);

    uint32_t total_pixels = (uint32_t)width * height;
    for (uint32_t i = 0; i < total_pixels; i++) {
        ILI9341_Write_Data(color);
    }
}

/* Optimized 8x16 Standard Character Drawing */
void ILI9341_DrawChar(uint16_t x, uint16_t y, char ch, uint16_t color, uint16_t bg_color) {
    if (ch < 32 || ch > 126) ch = ' ';
    uint16_t font_index = (ch - 32) * 16;

    for (uint8_t row = 0; row < 16; row++) {
        uint8_t b = Font8x16_Full[font_index + row];
        for (uint8_t col = 0; col < 8; col++) {
            if (b & (0x80 >> col)) {
                ILI9341_FillRectangle(x + col, y + row, 1, 1, color);
            } else {
                ILI9341_FillRectangle(x + col, y + row, 1, 1, bg_color);
            }
        }
    }
}

/* Draw String with Standard 8px Step */
void ILI9341_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg_color) {
    while (*str) {
        ILI9341_DrawChar(x, y, *str, color, bg_color);
        x += 9; // 8px font width + 1px spacing
        str++;
    }
}
