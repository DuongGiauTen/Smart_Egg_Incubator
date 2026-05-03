#include "task_core_iot.h"
#include "shared_data.h"
#include "task_check_info.h"
#include "task_wifi.h"
#include <ThingsBoard.h>
#include <Arduino_MQTT_Client.h>

constexpr uint32_t MAX_MESSAGE_SIZE = 1024U;
WiFiClient wifiClient;
Arduino_MQTT_Client mqttClient(wifiClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);

// =======================================================
// CÁC HÀM XỬ LÝ LỆNH TỪ CLOUD GỬI XUỐNG (RPC)
// =======================================================

// 1. Nhận lệnh chuyển đổi chế độ Auto / Manual
RPC_Response processSetMode(const RPC_Data &data)
{
    bool mode = data; // true là Auto, false là Manual
    set_auto_mode(mode);
    if (xSemaphoreTake(xSerialMutex, portMAX_DELAY))
    {
        Serial.printf(">> [RPC Cloud] Node 2 ra lenh chuyen che do: %s\n", mode ? "AUTO" : "MANUAL");
        xSemaphoreGive(xSerialMutex);
    }
    return RPC_Response("mode", mode);
}

// 2. Nhận lệnh chỉnh độ sáng đèn sưởi
RPC_Response processSetHeater(const RPC_Data &data)
{
    int val = data;
    // Chỉ cho phép Node 2 can thiệp nếu hệ thống đang tắt Auto
    if (!get_auto_mode())
    {
        set_heater_pwm(val);
        if (xSemaphoreTake(xSerialMutex, portMAX_DELAY))
        {
            Serial.printf(">> [RPC Cloud] Node 2 ra lenh chinh den: %d\n", val);
            xSemaphoreGive(xSerialMutex);
        }
    }
    return RPC_Response("heater", val);
}

// 3. Nhận lệnh quay máy đảo trứng (Servo)
RPC_Response processSetServo(const RPC_Data &data)
{
    int val = data;
    if (!get_auto_mode())
    {
        set_servo_angle(val);
        if (xSemaphoreTake(xSerialMutex, portMAX_DELAY))
        {
            Serial.printf(">> [RPC Cloud] Node 2 ra lenh quay Servo goc: %d\n", val);
            xSemaphoreGive(xSerialMutex);
        }
    }
    return RPC_Response("servo", val);
}

// Khai báo danh sách các lệnh để "nộp" cho CoreIoT
const size_t callbacks_size = 3;
RPC_Callback callbacks[callbacks_size] = {
    {"set_mode", processSetMode},
    {"set_heater", processSetHeater},
    {"set_servo", processSetServo}};

bool is_subscribed = false;

// =======================================================
// TASK CHÍNH CỦA CORE IOT
// =======================================================
void coreiot_task(void *pvParameters)
{
    Serial.println(">> Task CoreIoT Started! Cho mang Internet...");

    while (1)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    Serial.println(">> CoreIoT da nhan dien duoc Internet!");

    unsigned long last_send = 0;

    while (1)
    {
        String server = "";
        String token = "";
        String port_str = "";

        if (xSemaphoreTake(xConfigMutex, portMAX_DELAY))
        {
            server = String(sysConfig.iot_server);
            token = String(sysConfig.iot_token);
            port_str = String(sysConfig.iot_port);
            xSemaphoreGive(xConfigMutex);
        }

        if (server.isEmpty() || token.isEmpty())
        {
            vTaskDelay(pdMS_TO_TICKS(5000));
            continue;
        }

        if (!tb.connected())
        {
            is_subscribed = false;
            Serial.printf(">> Ket noi CoreIoT Server: %s, Port: %s...\n", server.c_str(), port_str.c_str());

            if (!tb.connect(server.c_str(), token.c_str(), port_str.toInt()))
            {
                Serial.println(">> Loi ket noi CoreIoT. Thu lai sau 5s...");
                vTaskDelay(pdMS_TO_TICKS(5000));
                continue;
            }
            Serial.println(">> Da ket noi CoreIOT Cloud thanh cong!");
        }

        // Đăng ký nhận lệnh từ Cloud
    // Ép kiểu rõ ràng thành con trỏ (Pointer) để C++ không bị nhầm lẫn
        RPC_Callback* first_callback = callbacks;
        RPC_Callback* last_callback = callbacks + callbacks_size;

        // Xin phép Cloud để lắng nghe 3 lệnh RPC đã khai báo ở trên
        if (!is_subscribed) {
            if (tb.RPC_Subscribe(first_callback, last_callback)) {
                if (xSemaphoreTake(xSerialMutex, portMAX_DELAY)) {
                    Serial.println(">> Da dang ky nhận lenh tu xa (RPC) thanh cong!");
                    xSemaphoreGive(xSerialMutex);
                }
                is_subscribed = true;
            }
        }

        tb.loop();

        // CHỈ BƠM DỮ LIỆU KHI TINYML CHO PHÉP (Kế thừa logic cũ)
        if (xSemaphoreTake(xDataReliableSemaphore, pdMS_TO_TICKS(10)) == pdTRUE)
        {

            float t = get_temperature();
            float h = get_humidity();
            bool mode = get_auto_mode();
            int heater = get_heater_pwm();
            int servo = get_servo_angle();

            if (t != -1 && h != -1)
            {
                // Báo cáo đầy đủ mọi trạng thái lên Cloud
                tb.sendTelemetryData("temperature", t);
                tb.sendTelemetryData("humidity", h);
                tb.sendTelemetryData("auto_mode", mode ? 1 : 0);
                tb.sendTelemetryData("heater_pwm", heater);
                tb.sendTelemetryData("servo_angle", servo);

                if (xSemaphoreTake(xSerialMutex, portMAX_DELAY))
                {
                    Serial.println(">> [CoreIoT] Da cap nhat trang thai len Cloud!");
                    xSemaphoreGive(xSerialMutex);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}