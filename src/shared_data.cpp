#include "shared_data.h"

// ===== BỔ SUNG 2 DÒNG NÀY ĐỂ CẤP PHÁT BỘ NHỚ =====
SystemConfig_t sysConfig;
SemaphoreHandle_t xConfigMutex = NULL;
// =================================================

static float safe_temperature = 0;
static float safe_humidity = 0;
uint8_t led_state = 0; // 0: Bình thường, 1: Cảnh báo, 2: Nguy hiểm
uint8_t humi_state = 0; // 0: Thấp, 1: Bình thường, 2: Cao

SemaphoreHandle_t xSensorDataMutex = NULL;
SemaphoreHandle_t xTempUpdateSemaphore = NULL;
SemaphoreHandle_t xHumiUpdateSemaphore = NULL; 
SemaphoreHandle_t xSerialMutex = NULL; 

// Khai báo biến
SemaphoreHandle_t xStateNormal = NULL;
SemaphoreHandle_t xStateWarning = NULL;
SemaphoreHandle_t xStateCritical = NULL;

SemaphoreHandle_t xDataReliableSemaphore = NULL; // Cờ báo dữ liệu đã sẵn sàng và đáng tin cậy để gửi lên Cloud

void init_shared_data() {
    xConfigMutex = xSemaphoreCreateMutex();
    // Khởi tạo giá trị mặc định cho sysConfig để tránh rác bộ nhớ
    if (xSemaphoreTake(xConfigMutex, portMAX_DELAY)) {
        memset(&sysConfig, 0, sizeof(SystemConfig_t));
        sysConfig.isConfigured = false;
        xSemaphoreGive(xConfigMutex);
        xDataReliableSemaphore = xSemaphoreCreateBinary(); // Khởi tạo cờ báo dữ liệu đáng tin cậy
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


int get_state_temp() {
    return led_state;
}
int get_state_humi() {
    return humi_state;
}

void set_state_temp(uint8_t tempState) {
    if(xSemaphoreTake(xSensorDataMutex, portMAX_DELAY)) {
        led_state = tempState;
        xSemaphoreGive(xSensorDataMutex);
    }
}

void set_state_humi(uint8_t humiState) {
    if(xSemaphoreTake(xSensorDataMutex, portMAX_DELAY)) {
        humi_state = humiState;
        xSemaphoreGive(xSensorDataMutex);
    }
}

// Thêm một biến cục bộ để lưu độ sáng (mặc định là 50 cho khỏi chói mắt lúc mới bật)
static int current_neo_brightness = 50; 

void set_neo_brightness(int brightness) {
    if(xSemaphoreTake(xSensorDataMutex, portMAX_DELAY)) {
        current_neo_brightness = brightness;
        xSemaphoreGive(xSensorDataMutex);
    }
}

int get_neo_brightness() {
    return current_neo_brightness;
}

