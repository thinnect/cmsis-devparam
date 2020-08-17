/**
 * SCD30 one point calibration and sensing parameters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_SCD30_PARAMS_H_
#define DEVP_SCD30_PARAMS_H_

#include "devp.h"

int16_t scd30_temp_offset ();

int16_t scd30_rh_offset ();

int16_t scd30_co2_offset ();

uint16_t scd30_warmup_get ();

void devp_scd30_params_init ();

#endif//DEVP_SCD30_PARAMS_H_
