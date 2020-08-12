/**
 * DS18B20 one point calibration parameters. Offsets to be applied to sensor values.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_DS18B20_PARAMS_H_
#define DEVP_DS18B20_PARAMS_H_

#include <stdint.h>

int16_t ds18b20_temp_offset ();

void devp_ds18b20_params_init();

#endif//DEVP_DS18B20_PARAMS_H_
