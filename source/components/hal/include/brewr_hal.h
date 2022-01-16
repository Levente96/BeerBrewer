#ifndef _BREWR_HAL_H_
#define _BREWR_HAL_H_

 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
#include "driver/adc.h"
#include "driver/gpio.h"
#include "esp_err.h"

#include "esp_log.h"
#include "esp_system.h"

#define GPIO_OUTPUT_HEATER_PIN    5         // NOTE: PIN 10 cannot be used as it is used for the "internal" flash resolder + reroute required
#define GPIO_OUTPUT_COOLER_LED    12
#define GPIO_OUTPUT_HEATER_LED    13
#define GPIO_OUTPUT_10V_PWM_OUT   16

#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_HEATER_PIN) | (1ULL<<GPIO_OUTPUT_COOLER_LED) | (1ULL<<GPIO_OUTPUT_HEATER_LED) | (1ULL<<GPIO_OUTPUT_10V_PWM_OUT))

int hal_sys_init();

void hal_heatingPinOn();

void hal_heatingPinOff();

void hal_heatingLedOn();

void hal_heatingLedOff();

void hal_coolingPinOn();

void hal_coolingPinOff();

void hal_coolingLedOn();

void hal_coolingLedOff();

int hal_getTemp();


#endif // _BREWR_HAL_H_