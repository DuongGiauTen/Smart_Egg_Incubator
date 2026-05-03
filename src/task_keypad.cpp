#include "task_keypad.h"
#include "global.h"
#include "shared_data.h"
#include <Keypad.h>

// Khai báo ma trận phím
char keys[ROW_NUM][COLUMN_NUM] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte pin_rows[ROW_NUM] = {8, 7, 6, 5};
byte pin_column[COLUMN_NUM] = {9, 10, 17, 18};

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

void task_keypad(void *pvParameters)
{
    // Khởi tạo chân output cho LED và Quạt

    while (1)
    {
        char key = keypad.getKey();
        if (key)
        {
            Serial.println("Key pressed: " + String(key));
        }
    }
    // Ngủ 50ms để chống dội phím (Debounce) và nhường CPU
    vTaskDelay(pdMS_TO_TICKS(50));
}
