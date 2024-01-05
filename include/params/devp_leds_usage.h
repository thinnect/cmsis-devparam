/**
 * Platform LED usage control through DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2024
 * @license MIT
 */
#ifndef DEVP_LEDS_USAGE_H_
#define DEVP_LEDS_USAGE_H_

#include <stdbool.h>

void devp_leds_usage_init (bool usage);
bool devp_leds_usage_get (void);

#endif//DEVP_LEDS_USAGE_H_
