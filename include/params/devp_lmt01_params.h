/**
 * LMT01 one point calibration parameters. Offsets to be applied to sensor values.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_LMT01_PARAMS_H_
#define DEVP_LMT01_PARAMS_H_

#include <stdint.h>

int16_t lmt01_temp_offset ();

void devp_lmt01_params_init();

#endif//DEVP_LMT01_PARAMS_H_
