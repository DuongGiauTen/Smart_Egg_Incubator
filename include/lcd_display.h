#ifndef __LCD_DISPLAY_H__
#define __LCD_DISPLAY_H__

#include <Arduino.h>
#include "LiquidCrystal_I2C.h"
#include "shared_data.h"

void lcd_display_task(void *pvParameters);

#endif