#ifndef __SHARED_DATA_H__
#define __SHARED_DATA_H__

#include <Arduino.h>
#include <FreeRTOS.h>
#include <semphr.h>

#define TEMP_LOW 0
#define TEMP_NORMAL 1
#define TEMP_HIGH 2

#define HUMI_LOW 4
#define HUMI_NORMAL 5
#define HUMI_HIGH 6

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

int get_state_temp();
int get_state_humi();
void set_state_temp(uint8_t tempState);
void set_state_humi(uint8_t humiState);
#endif