#include "shared_data.h"

static float safe_temperature = 0;
static float safe_humidity = 0;

SemaphoreHandle_t xSensorDataMutex = NULL;
SemaphoreHandle_t xTempUpdateSemaphore = NULL;
SemaphoreHandle_t xHumiUpdateSemaphore = NULL; // THÊM MỚI

// Khai báo biến
SemaphoreHandle_t xStateNormal = NULL;
SemaphoreHandle_t xStateWarning = NULL;
SemaphoreHandle_t xStateCritical = NULL;

void init_shared_data() {
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