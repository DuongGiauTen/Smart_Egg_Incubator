#include "mock.h"
#include "shared_data.h"
void mock_function(void *pvParameters)
{
    int counter = 0;
    int temp = 0;
    int humi = 0;
    while (1)
    {

        temp = temp + 5;
        humi = humi + 10;
        set_sensor_data(temp / 10.0, humi / 10.0);
        // Serial.print("[MOCK] Temperature: ");
        // Serial.print(get_temperature());
        // Serial.print("°C, Humidity: ");
        // Serial.print(get_humidity());
        // Serial.println("%");

        if (temp >800) temp = 0;
        if (humi > 1000) humi = 0;

        counter++;
        // Serial.print("[MOCK] Counter: ");
        // Serial.println(counter);
        if (counter >= 10)
        {
            set_sensor_data(40.5, 85.0); // Simulate anomaly
            vTaskDelay(1000);            // Wait a bit before resetting the counter
            counter = 0;
            // Serial.println("[MOCK] Simulating Anomaly: High Temperature and Humidity!");
        }

        if (get_temperature() > 30)
        {
            set_state_temp(TEMP_HIGH);
        }
        else if (get_temperature() < 25)
        {
            set_state_temp(TEMP_LOW);
        }
        else
        {
            set_state_temp(TEMP_NORMAL);
        }

        if (get_humidity() < 60)
        {
            set_state_humi(HUMI_LOW);
        }
        else if (get_humidity() > 80)
        {
            set_state_humi(HUMI_HIGH);
        }
        else
        {
            set_state_humi(HUMI_NORMAL);
        }
        vTaskDelay(1000);
    }
}