/**
 * DeviceParamater relative light sensor broadcast/resource behaviour control.
 *
 * Copyright Thinnect Inc. 2026
 * @license MIT
 */
#ifndef DEVP_LIGHT_BROADCAST_H_
#define DEVP_LIGHT_BROADCAST_H_

#include <stdbool.h>

void devp_light_broadcast_init (bool default_value);

bool devp_light_broadcast_get (void);

#endif//DEVP_LIGHT_BROADCAST_H_
