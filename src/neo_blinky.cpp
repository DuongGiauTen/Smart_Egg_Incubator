#include "neo_blinky.h"
#include "shared_data.h" // Thêm thư viện giao tiếp an toàn

void neo_blinky(void *pvParameters){
    Adafruit_NeoPixel strip(LED_COUNT, NEO_PIN, NEO_GRB + NEO_KHZ800);
    strip.begin();
    strip.clear();
    strip.show();
    strip.setBrightness(20);


    while(1) {
        // Task sẽ block (nằm chờ) ở đây cho đến khi Sensor đọc xong và "phất cờ"
        // portMAX_DELAY nghĩa là nó sẽ đợi vô hạn cho tới khi có cờ mới chạy tiếp
        if (xSemaphoreTake(xHumiUpdateSemaphore, portMAX_DELAY) == pdTRUE) {
            
            // Đã có cờ! Lấy giá trị độ ẩm một cách an toàn
            float current_humi = get_humidity();

            // Ánh xạ 3 mức độ ẩm với 3 màu sắc
            if (current_humi < 40.0) {
                // Mức 1: Độ ẩm thấp (< 40%) -> Màu Xanh Dương (Khô ráo)
                strip.setPixelColor(0, strip.Color(0, 0, 255));
                Serial.println("NeoPixel: LOW HUMIDITY -> BLUE");
                
            } else if (current_humi >= 40.0 && current_humi <= 70.0) {
                // Mức 2: Độ ẩm bình thường (40% - 70%) -> Màu Xanh Lá (Thoải mái)
                strip.setPixelColor(0, strip.Color(0, 255, 0));
                Serial.println("NeoPixel: NORMAL HUMIDITY -> GREEN");
                
            } else {
                // Mức 3: Độ ẩm cao (> 70%) -> Màu Đỏ (Ẩm ướt / Cảnh báo)
                strip.setPixelColor(0, strip.Color(255, 0, 0));
                Serial.println("NeoPixel: HIGH HUMIDITY -> RED");
            }
            
            // Cập nhật dải đèn LED với màu vừa set
            strip.show(); 
        }
    }
}