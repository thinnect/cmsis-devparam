/**
 * Current light level through deviceparameters.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#ifndef DEVP_LIGHT_LEVEL_RAW_H_
#define DEVP_LIGHT_LEVEL_RAW_H_

#include <stdint.h>

void devp_light_level_raw_init ();

void devp_light_level_raw_set (int16_t value);

#endif//DEVP_LIGHT_LEVEL_RAW_H_
