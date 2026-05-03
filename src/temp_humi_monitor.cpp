#include "temp_humi_monitor.h"
#include "shared_data.h"

void temp_humi_monitor(void *pvParameters) {
    Serial.println(">> [MONITOR] Task danh gia Nhiet/Am (Node 2) bat dau!");

    while (1) {
        // 1. Lấy dữ liệu trực tiếp từ "Két sắt" (dữ liệu này do CoreIoT lấy từ Cloud về)
        float temperature = get_temperature();
        float humidity = get_humidity();

        // Kiểm tra xem đã nhận được dữ liệu từ Cloud chưa (khác giá trị mặc định -1)
        if (temperature != -1 && humidity != -1) {
            
            // 2. Đánh giá trạng thái NHIỆT ĐỘ LÒ ẤP (Để Node 2 chớp đèn cảnh báo nếu cần)
            if (temperature > 30.0) {
                set_state_temp(TEMP_HIGH);
            } else if (temperature < 25.0) {
                set_state_temp(TEMP_LOW);
            } else {
                set_state_temp(TEMP_NORMAL);
            }

            // 3. Đánh giá trạng thái ĐỘ ẨM LÒ ẤP
            if (humidity < 60) {
                set_state_humi(HUMI_LOW);
            } else if (humidity > 70) {
                set_state_humi(HUMI_HIGH);
            } else {
                set_state_humi(HUMI_NORMAL);
            }
        }
        
        
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}