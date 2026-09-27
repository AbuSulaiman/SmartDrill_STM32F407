#ifndef __XPT2046_H
#define __XPT2046_H

#include "main.h"

typedef struct Touch_Point {
    uint16_t x;
    uint16_t y;
    uint8_t touched;
} Touch_Point;

uint8_t XPT2046_IsTouched(void);
uint8_t XPT2046_Get_Touch_Calibrated(Touch_Point *p);

#endif /* __XPT2046_H */
