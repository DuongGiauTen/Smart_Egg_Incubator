#include "lcd_display.h"
#include "shared_data.h"
#include "LiquidCrystal_I2C.h"

void lcd_display_task(void *pvParameters) {
    // Khởi tạo LCD ở đây (Trong file gốc là địa chỉ 33 tương đương 0x21)
    LiquidCrystal_I2C lcd(33, 16, 2); 
    lcd.init();
    lcd.backlight();
    
    while(1) {
        // Dùng if-else để kiểm tra xem cờ nào đang được phất
        // pdMS_TO_TICKS(10) là thời gian chờ tối đa 10ms để check cờ
        
        if (xSemaphoreTake(xStateNormal, pdMS_TO_TICKS(10)) == pdTRUE) {
            float t = get_temperature();
            float h = get_humidity();
            
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print("T:"); lcd.print(t); lcd.print("C H:"); lcd.print(h); lcd.print("%");
            lcd.setCursor(0, 1); lcd.print("St: NORMAL");
            
        } 
        else if (xSemaphoreTake(xStateWarning, pdMS_TO_TICKS(10)) == pdTRUE) {
            float t = get_temperature();
            float h = get_humidity();
            
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print("T:"); lcd.print(t); lcd.print("C H:"); lcd.print(h); lcd.print("%");
            lcd.setCursor(0, 1); lcd.print("St: WARNING");
            
        } 
        else if (xSemaphoreTake(xStateCritical, pdMS_TO_TICKS(10)) == pdTRUE) {
            float t = get_temperature();
            float h = get_humidity();
            
            lcd.clear();
            lcd.setCursor(0, 0); lcd.print("T:"); lcd.print(t); lcd.print("C H:"); lcd.print(h); lcd.print("%");
            lcd.setCursor(0, 1); lcd.print("St: CRITICAL");
        }
        
        // Nghỉ ngơi 100ms trước khi kiểm tra lại các cờ
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}