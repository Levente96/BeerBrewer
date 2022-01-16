#ifndef _TEMP_REGULATOR_H_
#define _TEMP_REGULATOR_H_

#include <stdint.h>
 #include "freertos/FreeRTOS.h"
 #include "freertos/task.h"
#include "driver/adc.h"

// Components
#include "brewr_hal.h"

#define LOW_TRIGGER_GATE  5
#define HIGH_TRIGGER_GATE 5
#define INTERVAL_SUM      10
#define RETRY_LIMIT       10

typedef enum 
{ 
    PREPARATION, 
    ACTIVE,
    PASSIVE,
    DONE 
} IntervalState;

typedef enum
{
    IDLE    = 0u,
    HEATING = 1u,
    COOLING = 2u
} IntervalRegulation;

typedef struct Interval
{
    IntervalState       state;              ///< State
    uint8_t             temp_target;        ///< Desired temperature to keep
    IntervalRegulation  regulation;         ///< Regulation type
    uint32_t            duration;           ///< Duration of the interval
    uint32_t            remaining;          ///< Remaining duration
} Interval;

static Interval tmp_reg_ci[INTERVAL_SUM];
TaskHandle_t    runner_Task;

void tmp_reg_temp_regulator_task(uint8_t index);

void temp_reg_interval_resolver(void);

static void tmp_reg_state_machine()
{
    Interval idle;
    idle.duration = 10;
    idle.remaining = 10;
    idle.regulation = IDLE;
    idle.state = DONE;
    for(uint8_t i = 0; i < INTERVAL_SUM; i++)
    {
        idle.regulation = (i % 2) ? HEATING : COOLING;
        tmp_reg_ci[i] = idle;
    }
    xTaskCreate(temp_reg_interval_resolver, "temp_guardian", 2048, NULL, 10, &runner_Task);
}

#endif // _TEMP_REGULATOR_H_