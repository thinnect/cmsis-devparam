/**
 * HDC2080 one point calibration and sensing parameters.
 *
 * Copyright Thinnect Inc. 2026
 * @license MIT
 */
#ifndef DEVP_HDC2080_PARAMS_H_
#define DEVP_HDC2080_PARAMS_H_

#include "devp.h"

int16_t hdc2080_temp_offset (void);
int16_t hdc2080_rh_offset (void);
void devp_hdc2080_params_init (void);

#endif//DEVP_HDC2080_PARAMS_H_
