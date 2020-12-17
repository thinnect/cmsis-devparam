/**
 * Generic "output" get/set through DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_OUTPUT_H_
#define DEVP_OUTPUT_H_

#include <stdint.h>

void devp_output_init (int8_t default_value);

int8_t devp_output_get ();

void devp_output_set (int8_t output);

#endif//DEVP_OUTPUT_H_
