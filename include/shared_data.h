#ifndef __SHARED_DATA_H__
#define __SHARED_DATA_H__

#include <Arduino.h>
#include <FreeRTOS.h>
#include <semphr.h>

extern SemaphoreHandle_t xSensorDataMutex;
extern SemaphoreHandle_t xTempUpdateSemaphore;
// THÊM MỚI: Cờ báo hiệu cho Task NeoPixel
extern SemaphoreHandle_t xHumiUpdateSemaphore; 

// Thêm vào dưới các cờ cũ
extern SemaphoreHandle_t xStateNormal;
extern SemaphoreHandle_t xStateWarning;
extern SemaphoreHandle_t xStateCritical;

void init_shared_data();
void set_sensor_data(float temp, float humi);
float get_temperature();
float get_humidity();

#endif