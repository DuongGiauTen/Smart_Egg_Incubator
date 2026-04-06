#include <task_handler.h>

// void handleWebSocketMessage(String message)
// {
//     Serial.println(message);
//     StaticJsonDocument<256> doc;

//     DeserializationError error = deserializeJson(doc, message);
//     if (error)
//     {
//         Serial.println("❌ Lỗi parse JSON!");
//         return;
//     }
//     JsonObject value = doc["value"];
//     if (doc["page"] == "device")
//     {
//         if (!value.containsKey("gpio") || !value.containsKey("status"))
//         {
//             Serial.println("⚠️ JSON thiếu thông tin gpio hoặc status");
//             return;
//         }

//         int gpio = value["gpio"];
//         String status = value["status"].as<String>();

//         Serial.printf("⚙️ Điều khiển GPIO %d → %s\n", gpio, status.c_str());
//         pinMode(gpio, OUTPUT);
//         if (status.equalsIgnoreCase("ON"))
//         {
//             digitalWrite(gpio, HIGH);
//             Serial.printf("🔆 GPIO %d ON\n", gpio);
//         }
//         else if (status.equalsIgnoreCase("OFF"))
//         {
//             digitalWrite(gpio, LOW);
//             Serial.printf("💤 GPIO %d OFF\n", gpio);
//         }
//     }
//     else if (doc["page"] == "setting")
//     {
//         // String WIFI_SSID = doc["value"]["ssid"].as<String>();
//         // String WIFI_PASS = doc["value"]["password"].as<String>();
//         // String CORE_IOT_TOKEN = doc["value"]["token"].as<String>();
//         // String CORE_IOT_SERVER = doc["value"]["server"].as<String>();
//         // String CORE_IOT_PORT = doc["value"]["port"].as<String>();

//         // Serial.println("📥 Nhận cấu hình từ WebSocket:");
//         // Serial.println("SSID: " + WIFI_SSID);
//         // Serial.println("PASS: " + WIFI_PASS);
//         // Serial.println("TOKEN: " + CORE_IOT_TOKEN);
//         // Serial.println("SERVER: " + CORE_IOT_SERVER);
//         // Serial.println("PORT: " + CORE_IOT_PORT);

//         // // 👉 Gọi hàm lưu cấu hình
//         // Save_info_File(WIFI_SSID, WIFI_PASS, CORE_IOT_TOKEN, CORE_IOT_SERVER, CORE_IOT_PORT);

//         // // Phản hồi lại client (tùy chọn)
//         // String msg = "{\"status\":\"ok\",\"page\":\"setting_saved\"}";
//         // ws.textAll(msg);
// JsonObject value = doc["value"];
//         String ssid = value["ssid"] | "";
//         String pass = value["password"] | "";
//         String dev_id = value["token"] | "ESP32_DEV"; // Lấy ô token làm Device ID
//         String server_url = value["server"] | "";
//         String port = value["port"] | "";

//         Serial.println(">> Nhan cau hinh moi tu Web. Dang luu...");
        
//         // GỌI HÀM SAVE VỚI ĐỦ 6 ĐỐI SỐ (Và truyền biến cục bộ vừa bóc tách ra)
//         Save_info_File(ssid, pass, dev_id, "TOKEN_FIXED", server_url, port);
//     }
// }
