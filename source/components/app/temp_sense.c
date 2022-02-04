#include "temp_sense.h"

uint16_t TEMP_SENSE_SHIFT = 0;

void TEMP_SENSE_init()
{
    adc_config_t adc_config;
    adc_config.mode = ADC_READ_TOUT_MODE;
    adc_config.clk_div = 8;
    ESP_ERROR_CHECK(adc_init(&adc_config));
}

uint8_t TEMP_SENSE_get_temp(uint16_t* temp)
{
    uint16_t t = 0;
    uint8_t ret_val = 1;
    if(adc_read(&t) == ESP_OK)
    {
        *temp = (t + TEMP_SENSE_SHIFT);
        ret_val = 0;
    }
    return ret_val;
}