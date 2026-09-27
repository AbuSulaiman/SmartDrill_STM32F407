#ifndef __ILI9341_H
#define __ILI9341_H

#include "main.h"

#define LCD_REG  (*((volatile uint16_t *)(0x60000000)))
#define LCD_DATA (*((volatile uint16_t *)(0x60080000)))

#define ILI9341_WIDTH   320
#define ILI9341_HEIGHT  240

#define COLOR_DARKGREY 0x4208
#define COLOR_BLUE     0x001F
#define COLOR_ORANGE   0xFD20
#define COLOR_RED      0xF800
#define COLOR_GREEN    0x07E0
#define COLOR_WHITE    0xFFFF
#define COLOR_BLACK    0x0000

void ILI9341_Init(void);
void ILI9341_FillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t color);
void Draw_Dashboard_UI(uint8_t fwd_state, uint8_t rev_state);
void ILI9341_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg_color);
void ILI9341_WriteString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg_color);

#endif /* __ILI9341_H */
