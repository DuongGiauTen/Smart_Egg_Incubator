#include "task_core_iot.h"
#include "shared_data.h"
#include "task_check_info.h"
#include "task_wifi.h"

// Khởi tạo đối tượng ThingsBoard
constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

void coreiot_task(void *pvParameters) {
    Serial.println(">> Task CoreIoT Started! Cho mang Internet...");

    // 1. ĐỢI HỆ THỐNG CÓ MẠNG INTERNET
    while (1) {
        if (WiFi.status() == WL_CONNECTED) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    Serial.println(">> CoreIoT da nhan dien duoc Internet!");

    unsigned long last_send = 0;

    while (1) {
        // 2. LẤY THÔNG TIN KẾT NỐI TỪ KÉT SẮT (Do WebServer của Task 4 lưu vào)
        Serial.printf("\n>> Da vao mang STA! IP: %s\n", WiFi.localIP().toString().c_str());
        String server = "";
        String token = "";
        String port_str = "";
        
        if (xSemaphoreTake(xConfigMutex, portMAX_DELAY)) {
            server = String(sysConfig.iot_server);
            token = String(sysConfig.iot_token);
            port_str = String(sysConfig.iot_port);
            xSemaphoreGive(xConfigMutex);
        }

        // Nếu Web chưa được cài đặt Server / Token -> Tạm ngủ 5s rồi check lại
        if (server.isEmpty() || token.isEmpty()) {
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        // 3. KẾT NỐI LÊN CLOUD BROKER (app.coreiot.io)
        if (!tb.connected()) {
            Serial.printf(">> Ket noi CoreIoT Server: %s, Port: %s...\n", server.c_str(), port_str.c_str());
            
            // Dùng Token lấy từ Web làm tham số kết nối
            if (!tb.connect(server.c_str(), token.c_str(), port_str.toInt())) {
                Serial.println(">> Loi ket noi CoreIoT. Thu lai sau 5s...");
                vTaskDelay(pdMS_TO_TICKS(5000));
                continue;
            }
            Serial.println(">> Da ket noi CoreIOT Cloud thanh cong!");
        }

        // Duy trì nhịp đập của MQTT
        tb.loop();

        // 4. BƠM DỮ LIỆU TELEMETRY LÊN CLOUD MỖI 5 GIÂY
        // if (millis() - last_send > 5000) {
        //     last_send = millis();
            
        //     // Lấy dữ liệu an toàn
        //     float t = get_temperature();
        //     float h = get_humidity();

        //     // Nếu dữ liệu hợp lệ thì bắn lên Cloud
        //     if (t != -1 && h != -1) {
        //         // Key "temperature" và "humidity" phải khớp với bảng Dashboard trên Web CoreIoT
        //         tb.sendTelemetryData("temperature", t);
        //         tb.sendTelemetryData("humidity", h);
                
        //         // In log an toàn qua Mutex
        //         if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
        //             Serial.printf(">> [CoreIoT] Da gui Cloud -> Temp: %.1f, Humi: %.1f\n", t, h);
        //             xSemaphoreGive(xSerialMutex);
        //         }
        //     }
        // }

        // 4. CHỈ BƠM DỮ LIỆU LÊN CLOUD KHI ĐƯỢC TINYML CHO PHÉP (Phất cờ)
        // Lệnh này sẽ chặn luồng lại, đứng chờ mãi mãi (portMAX_DELAY) cho đến khi AI bảo "OK"
        if (xSemaphoreTake(xDataReliableSemaphore, portMAX_DELAY) == pdTRUE) {
            
            // Lấy dữ liệu an toàn
            float t = get_temperature();
            float h = get_humidity();

            // Nếu dữ liệu hợp lệ thì bắn lên Cloud
            if (t != -1 && h != -1) {
                // Key "temperature" và "humidity" phải khớp với bảng Dashboard
                tb.sendTelemetryData("temperature", t);
                tb.sendTelemetryData("humidity", h);
                
                // In log an toàn qua Mutex
                if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
                    Serial.printf(">> [CoreIoT] Da gui Cloud -> Temp: %.1f, Humi: %.1f\n", t, h);
                    xSemaphoreGive(xSerialMutex);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50)); // Nhường CPU cho các Task khác
    }
}