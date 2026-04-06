#ifndef ___MAIN_SERVER__
#define ___MAIN_SERVER__
#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "global.h"

#define LED1_PIN 48
#define LED2_PIN 41
#define BOOT_PIN 0
//extern WebServer server;
extern String ssid;
extern String password;
extern String wifi_ssid;
extern String wifi_password;
extern float glob_temperature;
extern float glob_humidity;
extern SemaphoreHandle_t xBinarySemaphoreInternet;
//extern bool isAPMode;




String mainPage();
String settingsPage();

void startAP();
void setupServer();
void connectToWiFi();

void main_server_task(void *pvParameters);

#endif