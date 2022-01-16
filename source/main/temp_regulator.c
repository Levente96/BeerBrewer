#include "temp_regulator.h"


void tmp_reg_temp_regulator_task(uint8_t index)
{
    uint16_t temp = hal_getTemp();
    if(tmp_reg_ci[index].regulation != IDLE && tmp_reg_ci[index].state != DONE)
    {
        if(temp >= tmp_reg_ci[index].temp_target+HIGH_TRIGGER_GATE && tmp_reg_ci[index].state == ACTIVE)
        {
            tmp_reg_ci[index].state = PASSIVE;
            hal_heatingPinOff();
        }

        if(temp <= tmp_reg_ci[index].temp_target-LOW_TRIGGER_GATE && tmp_reg_ci[index].state == PASSIVE)
        {
            tmp_reg_ci[index].state = ACTIVE;
            hal_heatingPinOn();
        }
    }
    tmp_reg_ci[index].remaining--;
    printf("[%d/%d] temp is: %d, currently: %d\n", tmp_reg_ci[index].duration, tmp_reg_ci[index].duration-tmp_reg_ci[index].remaining, temp, tmp_reg_ci[index].state);
    // TODO: Log temperature
    
}

void temp_reg_interval_resolver(void)
{
    for(uint8_t i = 0; i < INTERVAL_SUM; i++)
    {
        tmp_reg_ci[i].state = PREPARATION;
        printf("[%d] interval preparation started\n", i);

        if(tmp_reg_ci[i].regulation == HEATING)
        {
            hal_heatingLedOn();
            hal_heatingPinOn();
        }

        if(tmp_reg_ci[i].regulation == COOLING)
        {
            hal_coolingLedOn();
            hal_coolingPinOn();
        }

        uint32_t retry_count = 0;
        while(hal_getTemp() <= tmp_reg_ci[i].temp_target+HIGH_TRIGGER_GATE &&
              retry_count < RETRY_LIMIT)
        {
            retry_count++;
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
        
        if(tmp_reg_ci[i].regulation == HEATING)
        {
            hal_heatingPinOff();
        }

        if(tmp_reg_ci[i].regulation == COOLING)
        {
            hal_coolingPinOff();
        }

        tmp_reg_ci[i].state = PASSIVE;
        printf("[%d] interval regular tempkeeping started\n", i);

        while(tmp_reg_ci[i].remaining > 0)
        {
            tmp_reg_temp_regulator_task(i);
            vTaskDelay(1000 / portTICK_RATE_MS);
        }
        tmp_reg_ci[i].state = DONE;
        hal_heatingLedOff();
        hal_heatingPinOff();
        hal_coolingLedOff();
        hal_coolingPinOff();
        printf("[%d] interval finished\n", i);
    }
    vTaskDelete(runner_Task);
}

