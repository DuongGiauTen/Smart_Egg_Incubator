#include "mock.h"
#include "shared_data.h"
void mock_function(void *pvParameters)
{

    while (1)
    {
        set_sensor_data(random(250, 350) / 10.0, random(300, 900) / 10.0);
        Serial.print("[MOCK] Temperature: ");
        Serial.print(get_temperature());
        Serial.print("°C, Humidity: ");
        Serial.print(get_humidity());
        Serial.println("%");
        vTaskDelay(1000);

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
    }
}