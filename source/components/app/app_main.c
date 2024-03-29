#include "app_main.h"

void APP_MAIN_init(void)
{
    APP_MAIN_cIntIndex      = 0u;
    APP_MAIN_paused         = 1u;
    APP_MAIN_time           = 0u;
    APP_MAIN_heating        = 0u;

    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
}

void APP_MAIN_start(void)
{
    for(;;)
    {
        uint16_t temp = 0xFFFF;
        // Read temperature if app is not paused
        if(TEMP_SENSE_get_temp(&temp) == 0u && APP_MAIN_paused == 0u)
        {
            ESP_LOGI("MAIN", "[%d/%ds]: %d°C/ %d°C",APP_MAIN_intervals[APP_MAIN_cIntIndex].duration, APP_MAIN_intervals[APP_MAIN_cIntIndex].length,
                                                    temp, APP_MAIN_intervals[APP_MAIN_cIntIndex].temp);
            // Turn ON heater if temp is lower than expected
            if(temp < APP_MAIN_intervals[APP_MAIN_cIntIndex].temp - TEMP_RANGE)
            {
                APP_MAIN_heating = 1u;
                gpio_set_level(GPIO_OUTPUT_HEATER_LED, 1);
                gpio_set_level(GPIO_OUTPUT_HEATER_PIN, 1);
            }
            // Turn OFF heater if upper limit is reached
            if(temp > APP_MAIN_intervals[APP_MAIN_cIntIndex].temp + TEMP_RANGE)
            {
                APP_MAIN_heating = 0u;
                gpio_set_level(GPIO_OUTPUT_HEATER_LED, 0);
                gpio_set_level(GPIO_OUTPUT_HEATER_PIN, 0);
            }

            // If duration is greater the we are preheating
            if(APP_MAIN_intervals[APP_MAIN_cIntIndex].duration > APP_MAIN_intervals[APP_MAIN_cIntIndex].length) 
            {
                // If temp is reached we are no longer preheating
                if(temp >= APP_MAIN_intervals[APP_MAIN_cIntIndex].temp) 
                {
                    ESP_LOGI("MAIN", "Preheat ended!");
                    APP_MAIN_intervals[APP_MAIN_cIntIndex].duration = APP_MAIN_intervals[APP_MAIN_cIntIndex].length;
                }
            }
            else
            {
                APP_MAIN_intervals[APP_MAIN_cIntIndex].duration -= APP_MAIN_LOOP_TIME;
            }
            if(APP_MAIN_intervals[APP_MAIN_cIntIndex].duration == 0)
            {
                // Go to the next interval
                APP_MAIN_cIntIndex++;
                if(APP_MAIN_cIntIndex >= (NUMBER_OF_INTERVALS - 1) || APP_MAIN_intervals[APP_MAIN_cIntIndex].length == 0)
                {   
                    // Brewing finished!
                    APP_MAIN_paused = 1u;
                }
            }

        }
        // If paused or temp could not be read
        else
        {
            if(APP_MAIN_paused == 0u)
            {
                ESP_LOGE("MAIN", "Temp could not be read!");
                // Thermal runaway!
                gpio_set_level(GPIO_OUTPUT_HEATER_LED, 1);
                gpio_set_level(GPIO_OUTPUT_COOLER_LED, 1);
                gpio_set_level(GPIO_OUTPUT_HEATER_PIN, 0);
            }
            else if(APP_MAIN_cIntIndex >= 9 || APP_MAIN_intervals[APP_MAIN_cIntIndex].length == 0)
            {
                ESP_LOGI("MAIN", "Finished/Stopped!");
                gpio_set_level(GPIO_OUTPUT_HEATER_LED, 0);
                gpio_set_level(GPIO_OUTPUT_COOLER_LED, 0);
                gpio_set_level(GPIO_OUTPUT_HEATER_PIN, 0);
            }
        }
        vTaskDelayUntil(&APP_MAIN_time, (TickType_t)(APP_MAIN_LOOP_TIME*1000/portTICK_PERIOD_MS));    // Loop every 10 seconds
    }
}