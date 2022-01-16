#include "brewr_hal.h"

void hal_heatingPinOn()
{
    ESP_LOGI("SYS_HAL", "Heating is ON!");
    gpio_set_level(GPIO_OUTPUT_HEATER_PIN, 1);
}

void hal_heatingPinOff()
{
    ESP_LOGI("SYS_HAL", "Heating is OFF!");
    gpio_set_level(GPIO_OUTPUT_HEATER_PIN, 0);
}

void hal_heatingLedOn()
{
    gpio_set_level(GPIO_OUTPUT_HEATER_LED, 1);
}

void hal_heatingLedOff()
{
    gpio_set_level(GPIO_OUTPUT_HEATER_LED, 0);
}

void hal_coolingPinOn()
{
    ESP_LOGI("SYS_HAL", "Heating is ON!");
    gpio_set_level(GPIO_OUTPUT_HEATER_PIN, 1);
}

void hal_coolingPinOff()
{
    ESP_LOGI("SYS_HAL", "Heating is OFF!");
    gpio_set_level(GPIO_OUTPUT_HEATER_PIN, 0);
}

void hal_coolingLedOn()
{
    gpio_set_level(GPIO_OUTPUT_COOLER_LED, 1);
}

void hal_coolingLedOff()
{
    gpio_set_level(GPIO_OUTPUT_COOLER_LED, 0);
}

int hal_getTemp()
{
    uint16_t temp = 0;
    adc_read(&temp);
    return temp;
}

int hal_sys_init()
{
    ESP_LOGI("SYS_HAL", "Initialization ADC...");
    adc_config_t adc_config;
    adc_config.mode = ADC_READ_TOUT_MODE;
    adc_config.clk_div = 8;
    ESP_ERROR_CHECK(adc_init(&adc_config));
    ESP_LOGI("SYS_HAL", "Initialization ADC: Done");

    ESP_LOGI("SYS_HAL", "Initialization of GPIOs...");
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = GPIO_OUTPUT_PIN_SEL;
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    ESP_LOGI("SYS_HAL", "Initialization of GPIOs: Done");
    return ESP_OK;
}