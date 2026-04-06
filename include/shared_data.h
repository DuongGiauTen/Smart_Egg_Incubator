#ifndef __SHARED_DATA_H__
#define __SHARED_DATA_H__

#include <Arduino.h>
#include <FreeRTOS.h>
#include <semphr.h>

typedef struct {
    char wifi_ssid[32];
    char wifi_pass[64];
    char device_id[32]; // định danh cho GateWay
    char iot_token[64];
    char iot_server[64];
    char iot_port[10];
    bool isConfigured; //  cờ báo xem file đã cấu hình hay chưa
}SystemConfig_t;

//quản lí độ sáng led NEO(task 4)
void set_neo_brightness(int brightness);
int get_neo_brightness();

// Trong shared_data.h
extern SystemConfig_t sysConfig;
extern SemaphoreHandle_t xConfigMutex;


// Khai báo biến toàn cục (SẼ ĐƯỢC BẢO VỆ BỞI MUTEX)
extern SystemConfig_t sysConfig;
extern SemaphoreHandle_t xConfigMutex;

extern SemaphoreHandle_t xSensorDataMutex;
extern SemaphoreHandle_t xTempUpdateSemaphore;
// THÊM MỚI: Cờ báo hiệu cho Task NeoPixel
extern SemaphoreHandle_t xHumiUpdateSemaphore; 

// Thêm vào dưới các cờ cũ
extern SemaphoreHandle_t xStateNormal;
extern SemaphoreHandle_t xStateWarning;
extern SemaphoreHandle_t xStateCritical;

extern SemaphoreHandle_t xSerialMutex; // Thêm dòng này(mutex để in lần lượt trên Serial)

void init_shared_data();
void set_sensor_data(float temp, float humi);
float get_temperature();
float get_humidity();

#endif