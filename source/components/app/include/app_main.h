/**
 * \file app_main.h
 * \author Levente Csoka (levente.csoka96@gmail.com)
 * \brief 
 * \version 0.1
 * \date 2022-02-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _APP_MAIN_H_
#define _APP_MAIN_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include <stdint.h>

// Components
#include "temp_sense.h"

// NOTE: PIN 10 cannot be used as it is used for the "internal" flash resolder + reroute required
#define GPIO_OUTPUT_HEATER_PIN    5         
#define GPIO_OUTPUT_COOLER_LED    12
#define GPIO_OUTPUT_HEATER_LED    13
#define GPIO_OUTPUT_10V_PWM_OUT   16

#define TEMP_RANGE                5
#define APP_MAIN_LOOP_TIME        10


#define GPIO_OUTPUT_PIN_SEL  ((1ULL<<GPIO_OUTPUT_HEATER_PIN) | (1ULL<<GPIO_OUTPUT_COOLER_LED) | (1ULL<<GPIO_OUTPUT_HEATER_LED) | (1ULL<<GPIO_OUTPUT_10V_PWM_OUT))

/**
 * \brief Interval to maintain
 * 
 */
typedef struct interval
{
    uint32_t length;        ///< Length in seconds
    uint32_t duration;      ///< Duration left in interval
    uint8_t  temp;          ///< Temperature
}interval;

interval        APP_MAIN_intervals[10]; ///< Intervals of brewing
uint8_t         APP_MAIN_cIntIndex ;    ///< Index of current interval
uint8_t         APP_MAIN_paused;        ///< Should the app be running
TickType_t      APP_MAIN_time;          ///< Timer for periodic time call
TaskHandle_t    APP_MAIN_th;            ///< Main APP Task handler
uint8_t         APP_MAIN_heating;       ///< Flag if the heater is running

/**
 * \brief Main app init
 * 
 */
void APP_MAIN_init(void);

/**
 * \brief Main app loop
 * 
 */
void APP_MAIN_start(void);



#endif