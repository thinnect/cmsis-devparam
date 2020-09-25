/**
 * Default noise sensor parameters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_NOISE_PARAMS_H_
#define DEVP_NOISE_PARAMS_H_

#include "devp.h"

/**
 * Initialize noise sensor parameters.
 */
void devp_noise_params_init ();

/**
 * Get noise sensor OFFSET.
 *
 * @return OFFSET.
 */
uint8_t devp_noise_params_get_offset ();

#endif
