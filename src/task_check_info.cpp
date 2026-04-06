#include "task_check_info.h"
#include "shared_data.h"

void Load_info_File()
{
  File file = LittleFS.open("/info.dat", "r");
  if (!file)
  {
    // Nếu không có file, gán cờ isConfigured = false
    if (xSemaphoreTake(xConfigMutex, portMAX_DELAY)) {
        sysConfig.isConfigured = false;
        xSemaphoreGive(xConfigMutex);
    }
    return;
  }
  DynamicJsonDocument doc(4096);
  DeserializationError error = deserializeJson(doc, file);
  if (error)
  {
    Serial.print(F("deserializeJson() failed: "));
  }
  else
  {
    // WIFI_SSID = strdup(doc["WIFI_SSID"]);
    // WIFI_PASS = strdup(doc["WIFI_PASS"]);
    // CORE_IOT_TOKEN = strdup(doc["CORE_IOT_TOKEN"]);
    // CORE_IOT_SERVER = strdup(doc["CORE_IOT_SERVER"]);
    // CORE_IOT_PORT = strdup(doc["CORE_IOT_PORT"]);
    // Ghi vào struct và khóa Mutex
    if (xSemaphoreTake(xConfigMutex, portMAX_DELAY)) {
        strlcpy(sysConfig.wifi_ssid, doc["WIFI_SSID"] | "", sizeof(sysConfig.wifi_ssid));
        strlcpy(sysConfig.wifi_pass, doc["WIFI_PASS"] | "", sizeof(sysConfig.wifi_pass));
        strlcpy(sysConfig.device_id, doc["DEVICE_ID"] | "ESP32_UNNAMED", sizeof(sysConfig.device_id)); // Đọc Device ID
        strlcpy(sysConfig.iot_token, doc["CORE_IOT_TOKEN"] | "", sizeof(sysConfig.iot_token));
        strlcpy(sysConfig.iot_server, doc["CORE_IOT_SERVER"] | "", sizeof(sysConfig.iot_server));
        strlcpy(sysConfig.iot_port, doc["CORE_IOT_PORT"] | "", sizeof(sysConfig.iot_port));
        
        sysConfig.isConfigured = (strlen(sysConfig.wifi_ssid) > 0);
        xSemaphoreGive(xConfigMutex);
    }
  }
  file.close();
}

void Delete_info_File()
{
  if (LittleFS.exists("/info.dat"))
  {
    LittleFS.remove("/info.dat");
  }
  ESP.restart();
}

void Save_info_File(String wifi_ssid, String wifi_pass,String device_id , String CORE_IOT_TOKEN, String CORE_IOT_SERVER, String CORE_IOT_PORT)
{
  Serial.println("Dang luu SSID: " + wifi_ssid);
  Serial.println("Dang luu Device ID: " + device_id);
  // Serial.println(wifi_ssid);
  // Serial.println(wifi_pass);

  DynamicJsonDocument doc(4096);
  doc["WIFI_SSID"] = wifi_ssid;
  doc["WIFI_PASS"] = wifi_pass;
  doc["DEVICE_ID"] = device_id; // Thêm Device ID mặc định
  doc["CORE_IOT_TOKEN"] = CORE_IOT_TOKEN;
  doc["CORE_IOT_SERVER"] = CORE_IOT_SERVER;
  doc["CORE_IOT_PORT"] = CORE_IOT_PORT;

  File configFile = LittleFS.open("/info.dat", "w");
  if (configFile)
  {
    serializeJson(doc, configFile);
    configFile.close();
    Serial.println("Da luu cau hinh vao Flash!");
  }
  else
  {
    //Serial.println('Unable to save the configuration.');
    Serial.println("Loi: Khong the mo file de ghi.");
  }
  ESP.restart();
};

bool check_info_File(bool check)
{
  if (!check)
  {
    if (!LittleFS.begin(true))
    {
      Serial.println("❌ Lỗi khởi động LittleFS!");
      return false;
    }
    Load_info_File();
  }
  
  // if (WIFI_SSID.isEmpty() && WIFI_PASS.isEmpty())
  // {
  //   if (!check)
  //   {
  //     startAP();
  //   }
  //   return false;
  // }
  bool isConf = false;
  if (xSemaphoreTake(xConfigMutex, portMAX_DELAY)) {
      isConf = sysConfig.isConfigured;
      xSemaphoreGive(xConfigMutex);
  }

  // Nếu chưa có file (hoặc WiFi rỗng)
  if (!isConf) 
  {
    if (!check) // Chỉ gọi startAP lúc khởi động, tránh gọi liên tục trong loop
    {
      Serial.println("[INFO] Chua co cau hinh Wi-Fi -> Chuyen sang AP Mode!");
      startAP(); 
    }
    return false;
  }
  return true;
}