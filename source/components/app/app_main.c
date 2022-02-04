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
        if(TEMP_SENSE_get_temp(&temp) == 0u && APP_MAIN_paused == 0u)
        {
            if(temp < APP_MAIN_intervals[APP_MAIN_cIntIndex].temp - TEMP_RANGE)
            {
                APP_MAIN_heating = 1u;
                gpio_set_level(GPIO_OUTPUT_HEATER_LED, 1);
                gpio_set_level(GPIO_OUTPUT_HEATER_PIN, 1);
            }
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
                if(APP_MAIN_cIntIndex >= 9)
                {   
                    // Brewing finished!
                    APP_MAIN_paused = 1u;
                }
            }

        }
        else
        {
            if(APP_MAIN_paused == 0u)
            {
                // LOG ERROR
                gpio_set_level(GPIO_OUTPUT_HEATER_LED, 1);
                gpio_set_level(GPIO_OUTPUT_COOLER_LED, 1);
                gpio_set_level(GPIO_OUTPUT_HEATER_PIN, 0);
            }
        }
        vTaskDelayUntil(&APP_MAIN_time, (TickType_t)(APP_MAIN_LOOP_TIME*1000/portTICK_PERIOD_MS));    // Loop every 10 seconds
    }
}