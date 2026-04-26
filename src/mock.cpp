#include "mock.h"
#include "shared_data.h"
void mock_function(void *pvParameters)
{
    int counter = 0;
    while (1)
    {
        set_sensor_data(random(250, 260) / 10.0, random(600, 610) / 10.0);
        // Serial.print("[MOCK] Temperature: ");
        // Serial.print(get_temperature());
        // Serial.print("°C, Humidity: ");
        // Serial.print(get_humidity());
        // Serial.println("%");

        counter++;
        Serial.print("[MOCK] Counter: ");
        Serial.println(counter);
        if (counter >= 10)
        {
            //set_sensor_data(40.5, 85.0); // Simulate anomaly
            //vTaskDelay(1000); // Wait a bit before resetting the counter
            counter = 0;
            Serial.println("[MOCK] Simulating Anomaly: High Temperature and Humidity!");
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