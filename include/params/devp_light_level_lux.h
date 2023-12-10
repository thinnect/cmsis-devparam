/**
 * Current light level through deviceparameters.
 *
 * Copyright Thinnect Inc. 2023
 * @license MIT
 */
#ifndef DEVP_LIGHT_LEVEL_LUX_H_
#define DEVP_LIGHT_LEVEL_LUX_H_

#include <stdint.h>

void devp_light_level_lux_init ();

void devp_light_level_lux_set (int32_t value);

#endif//DEVP_LIGHT_LEVEL_LUX_H_
