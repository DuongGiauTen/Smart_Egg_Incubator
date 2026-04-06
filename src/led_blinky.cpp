#include "led_blinky.h"
#include "shared_data.h"

void led_blinky(void *pvParameters){
    pinMode(LED_GPIO, OUTPUT);
    int blink_delay = 1000; // Mặc định nháy chậm

    while(1) {
        // Chờ tín hiệu từ Sensor Task báo có nhiệt độ mới. 
        // Nếu sau 10 Tick không có cờ, lệnh này sẽ bỏ qua và tiếp tục giữ nguyên delay cũ để duy trì nhịp nháy.
        if (xSemaphoreTake(xTempUpdateSemaphore, pdMS_TO_TICKS(10)) == pdTRUE) {
            
            // Đã có cờ hiệu! Tiến hành lấy nhiệt độ an toàn
            float current_temp = get_temperature();
            if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
                // Định nghĩa 3 hành vi theo nhiệt độ
                if (current_temp < 28.0) {
                    blink_delay = 1000; // < 28 độ: Bình thường -> Nháy chậm (Chu kỳ 2s)
                    Serial.println("LED State: NORMAL");
                } else if (current_temp >= 28.0 && current_temp <= 32.0) {
                    blink_delay = 500;  // 28 - 32 độ: Cảnh báo -> Nháy vừa (Chu kỳ 1s)
                    Serial.println("LED State: WARNING");
                } else {
                    blink_delay = 100;  // > 32 độ: Nguy hiểm -> Nháy nhanh (Chu kỳ 0.2s)
                    Serial.println("LED State: CRITICAL");
                }
            xSemaphoreGive(xSerialMutex); // Nhả khóa
            }
        }

        // Thực thi việc nháy LED với delay đã được tính toán
        digitalWrite(LED_GPIO, HIGH);
        vTaskDelay(pdMS_TO_TICKS(blink_delay));
        digitalWrite(LED_GPIO, LOW);
        vTaskDelay(pdMS_TO_TICKS(blink_delay));
    }
}