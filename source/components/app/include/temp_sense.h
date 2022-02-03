/**
 * \file temp_sense.h
 * \author Levente Csoka (levente.csoka96@gmail.com)
 * \brief Temperature sensor handler
 * \version 0.1
 * \date 2022-02-02
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifndef _TEMP_SENSE_
#define _TEMP_SENSE_

#include <stdint.h>
#include "driver/adc.h"
#include "esp_err.h"

uint16_t TEMP_SENSE_SHIFT = 0;  ///<    Correction for the temperature

/**
 * \brief Init temperature sensing
 * 
 */
void TEMP_SENSE_init();

/**
 * \brief Get current temperature
 * 
 * \param temp Temperature will be stored here
 * \return uint8_t Error code
 */
uint8_t TEMP_SENSE_get_temp(uint16_t* temp);

#endif