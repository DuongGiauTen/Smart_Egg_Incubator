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
            uint8_t humi_state = get_state_humi(); // Lấy trạng thái độ ẩm đã được Task Sensor đánh giá


            // Ánh xạ 3 mức độ ẩm với 3 màu sắc
            switch (humi_state) {
                case HUMI_LOW:
                    strip.setPixelColor(0, strip.Color(0, 0, 255)); // Độ ẩm thấp -> Xanh dương
                    Serial.println("NEO: LOW HUMIDITY -> BLUE");
                    break;
                case HUMI_NORMAL:
                    strip.setPixelColor(0, strip.Color(255, 255, 0)); // Độ ẩm bình thường -> Xanh lá
                    Serial.println("NEO: NORMAL HUMIDITY -> YELLOW");
                    break;
                case HUMI_HIGH:
                    strip.setPixelColor(0, strip.Color(255, 0, 0)); // Độ ẩm cao -> Đỏ
                    Serial.println("NEO: HIGH HUMIDITY -> RED");
                    break;
                default:
                    strip.setPixelColor(0, strip.Color(255, 255, 255)); // Mặc định nếu có lỗi trạng thái -> Trắng
                    Serial.println("NEO: UNKNOWN HUMIDITY STATE -> WHITE");
            }

            // LẤY ĐỘ SÁNG TỪ WEB (thông qua shared_data) VÀ ÁP DỤNG NGAY
            int brightness = get_neo_brightness();
            strip.setBrightness(brightness);

            
        //     // Dùng Mutex khi in log Serial
        //     if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
        //         // Ánh xạ 3 mức độ ẩm với 3 màu sắc
        //         if (current_humi < 40.0) {
        //             // Mức 1: Độ ẩm thấp (< 40%) -> Màu Xanh Dương (Khô ráo)
        //             strip.setPixelColor(0, strip.Color(0, 0, 255));
        //             Serial.println("NeoPixel: LOW HUMIDITY -> BLUE");
                    
        //         } else if (current_humi >= 40.0 && current_humi <= 70.0) {
        //             // Mức 2: Độ ẩm bình thường (40% - 70%) -> Màu Xanh Lá (Thoải mái)
        //             strip.setPixelColor(0, strip.Color(0, 255, 0));
        //             Serial.println("NeoPixel: NORMAL HUMIDITY -> GREEN");
                    
        //         } else {
        //             // Mức 3: Độ ẩm cao (> 70%) -> Màu Đỏ (Ẩm ướt / Cảnh báo)
        //             strip.setPixelColor(0, strip.Color(255, 0, 0));
        //             Serial.println("NeoPixel: HIGH HUMIDITY -> RED");
        //         }
        //     xSemaphoreGive(xSerialMutex); // Nhả khóa sau khi in log
        // }

            // Cập nhật dải đèn LED với màu vừa set
            strip.show(); 
        }
    }
}