/**
 * SCD4x offset parameters
 *
 * Copyright Thinnect Inc. 2026
 * @license MIT
 */
#ifndef DEVP_SCD4X_PARAMS_H_
#define DEVP_SCD4X_PARAMS_H_

#include "devp.h"

int16_t scd4x_temp_offset (void);
int16_t scd4x_rh_offset (void);
void devp_scd4x_params_init (void);

#endif//DEVP_SCD4X_PARAMS_H_
