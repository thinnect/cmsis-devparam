/**
 * DeviceParamater absolute light sensor broadcast/resource behaviour control.
 *
 * Copyright Thinnect Inc. 2023
 * @license MIT
 */
#ifndef DEVP_LUX_BROADCAST_H_
#define DEVP_LUX_BROADCAST_H_

#include <stdbool.h>

void devp_lux_broadcast_init (bool default_value);

bool devp_lux_broadcast_get (void);

#endif//DEVP_LUX_BROADCAST_H_
