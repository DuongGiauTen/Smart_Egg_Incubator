#include "shared_data.h"

// ===== BỔ SUNG 2 DÒNG NÀY ĐỂ CẤP PHÁT BỘ NHỚ =====
SystemConfig_t sysConfig;
SemaphoreHandle_t xConfigMutex = NULL;
// =================================================

static float safe_temperature = 0;
static float safe_humidity = 0;

SemaphoreHandle_t xSensorDataMutex = NULL;
SemaphoreHandle_t xTempUpdateSemaphore = NULL;
SemaphoreHandle_t xHumiUpdateSemaphore = NULL; // THÊM MỚI
SemaphoreHandle_t xSerialMutex = NULL; // Thêm dòng này

// Khai báo biến
SemaphoreHandle_t xStateNormal = NULL;
SemaphoreHandle_t xStateWarning = NULL;
SemaphoreHandle_t xStateCritical = NULL;

void init_shared_data() {
    xConfigMutex = xSemaphoreCreateMutex();
    // Khởi tạo giá trị mặc định cho sysConfig để tránh rác bộ nhớ
    if (xSemaphoreTake(xConfigMutex, portMAX_DELAY)) {
        memset(&sysConfig, 0, sizeof(SystemConfig_t));
        sysConfig.isConfigured = false;
        xSemaphoreGive(xConfigMutex);
    }

    xSensorDataMutex = xSemaphoreCreateMutex();
    xTempUpdateSemaphore = xSemaphoreCreateBinary();
    xHumiUpdateSemaphore = xSemaphoreCreateBinary();
    
    // KHỞI TẠO 3 CỜ TRẠNG THÁI CHO LCD
    xStateNormal = xSemaphoreCreateBinary();
    xStateWarning = xSemaphoreCreateBinary();
    xStateCritical = xSemaphoreCreateBinary();
}


void set_sensor_data(float temp, float humi) {
    if (xSemaphoreTake(xSensorDataMutex, portMAX_DELAY)) {
        safe_temperature = temp;
        safe_humidity = humi;
        xSemaphoreGive(xSensorDataMutex); 

        // Báo hiệu cho cả 2 Task biết đã có dữ liệu mới
        xSemaphoreGive(xTempUpdateSemaphore); // Cho Task LED
        xSemaphoreGive(xHumiUpdateSemaphore); // Cho Task NeoPixel
    }
}

float get_temperature() {
    float temp = -1;
    if (xSemaphoreTake(xSensorDataMutex, portMAX_DELAY)) {
        temp = safe_temperature;
        xSemaphoreGive(xSensorDataMutex);
    }
    return temp;
}

float get_humidity() {
    float humi = -1;
    if (xSemaphoreTake(xSensorDataMutex, portMAX_DELAY)) {
        humi = safe_humidity;
        xSemaphoreGive(xSensorDataMutex);
    }
    return humi;
}

// Thêm một biến cục bộ để lưu độ sáng (mặc định là 50 cho khỏi chói mắt lúc mới bật)
static int current_neo_brightness = 50; 

void set_neo_brightness(int brightness) {
    current_neo_brightness = brightness;
}

int get_neo_brightness() {
    return current_neo_brightness;
}