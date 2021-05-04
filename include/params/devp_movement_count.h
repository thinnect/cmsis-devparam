/**
 * Detected movement count over DeviceParameters.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#ifndef DEVP_MOVEMENT_COUNT_H_
#define DEVP_MOVEMENT_COUNT_H_

#include <stdint.h>

void devp_movement_count_init ();

void devp_movement_count_increase ();

int32_t devp_movement_count_get ();

#endif//DEVP_MOVEMENT_COUNT_H_
