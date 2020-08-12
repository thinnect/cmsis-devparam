/**
 * BME280 one point calibration parameters. Offsets to be applied to sensor values.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_BME280_PARAMS_H_
#define DEVP_BME280_PARAMS_H_

#include <stdint.h>

int16_t bme280_temp_offset ();
int16_t bme280_rh_offset ();
int16_t bme280_pressure_offset ();

void devp_bme280_params_init();

#endif//DEVP_BME280_PARAMS_H_
