/**
 * DeviceParamater absolute light sensor value correction offset.
 *
 * Copyright Thinnect Inc. 2023
 * @license MIT
 */
#ifndef DEVP_LUX_OFFSET_H_
#define DEVP_LUX_OFFSET_H_

#include <stdint.h>

void devp_lux_offset_init (int16_t default_value);

int16_t devp_lux_offset_get (void);

#endif//DEVP_LUX_OFFSET_H_
