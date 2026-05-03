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

#define SERVO_PIN       38 
#define LED_PIN         48

#define ROW_NUM         4 
#define COLUMN_NUM      4

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

extern SemaphoreHandle_t xDataReliableSemaphore; //  cờ báo dữ liệu đã sẵn sàng và đáng tin cậy để gửi lên Cloud

void init_shared_data();
void set_sensor_data(float temp, float humi);
float get_temperature();
float get_humidity();

int get_state_temp();
int get_state_humi();
void set_state_temp(uint8_t tempState);
void set_state_humi(uint8_t humiState);


// ==========================================
// CÁC HÀM BỔ SUNG CHO LÒ ẤP TRỨNG (NODE 1)
// ==========================================
// 1. Chế độ hoạt động (true = Auto, false = Manual)
void set_auto_mode(bool mode);
bool get_auto_mode();

// 2. Độ sáng đèn sưởi giả lập (0 - 255)
void set_heater_pwm(int pwm);
int get_heater_pwm();

// 3. Góc quay máy đảo trứng (0 - 180 độ)
void set_servo_angle(int angle);
int get_servo_angle();
#endif