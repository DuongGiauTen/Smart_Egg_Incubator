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

// --- 1. NGHE DỮ LIỆU TỪ MÂY ĐỂ HIỂN THỊ LCD ---
RPC_Response processUpdateTemp(const RPC_Data &data)
{
    float temp = data;
    set_sensor_data(temp, get_humidity()); // Cập nhật Temp, giữ nguyên Humi
    return RPC_Response("update_temp", temp);
}

RPC_Response processUpdateHumi(const RPC_Data &data)
{
    float humi = data;
    set_sensor_data(get_temperature(), humi); // Cập nhật Humi, giữ nguyên Temp
    return RPC_Response("update_humi", humi);
}

const size_t callbacks_size = 2;
RPC_Callback callbacks[callbacks_size] = {
    {"update_temp", processUpdateTemp},
    {"update_humi", processUpdateHumi}};

bool is_subscribed = false;

// --- 2. TASK CHÍNH ---
void coreiot_task(void *pvParameters)
{
    Serial.println(">> [Node 2] Task CoreIoT Started!");

    while (WiFi.status() != WL_CONNECTED)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    while (1)
    {
        String server = "", token = "", port_str = "";

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
            if (!tb.connect(server.c_str(), token.c_str(), port_str.toInt()))
            {
                vTaskDelay(pdMS_TO_TICKS(5000));
                continue;
            }
        }

        RPC_Callback *first_callback = callbacks;
        RPC_Callback *last_callback = callbacks + callbacks_size;

        if (!is_subscribed)
        {
            // 1. Dọn dẹp rác bộ nhớ của lần thất bại trước (nếu có)
            tb.RPC_Unsubscribe();

            if (xSemaphoreTake(xSerialMutex, portMAX_DELAY))
            {
                Serial.println(">> Dang gui yeu cau dang ky RPC len Cloud...");
                xSemaphoreGive(xSerialMutex);
            }

            // 2. Thử đăng ký lại
            if (tb.RPC_Subscribe(first_callback, last_callback))
            {
                is_subscribed = true;
                if (xSemaphoreTake(xSerialMutex, portMAX_DELAY))
                {
                    Serial.println(">> Da mo RPC san sang nghe du lieu!");
                    xSemaphoreGive(xSerialMutex);
                }
            }
            else
            {
                // 3. NẾU THẤT BẠI: Bắt buộc phải ngủ 3 giây rồi mới thử lại, cấm spam!
                if (xSemaphoreTake(xSerialMutex, portMAX_DELAY))
                {
                    Serial.println(">> Dang ky RPC that bai. Thu lai sau 3 giay...");
                    xSemaphoreGive(xSerialMutex);
                }
                vTaskDelay(pdMS_TO_TICKS(3000));
            }
        }
    }
}