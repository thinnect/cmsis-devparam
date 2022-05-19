/**
 * Default SM3 MW sensor speed parameters.
 *
 * Copyright Thinnect Inc. 2022
 * @license MIT
 */
#ifndef DEVP_SPEED_PARAMS_H_
#define DEVP_SPEED_PARAMS_H_

#include "devp.h"

/**
 * Initialize speed sensor parameters.
 */
void devp_speed_params_init ();

/**
 * Get SM3 MW sensor speed limit.
 *
 * @return LIMIT.
 */
uint8_t devp_speed_params_get_limit ();

#endif
