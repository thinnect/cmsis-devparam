/**
 * PIR hardware connection status through deviceparameters.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#ifndef DEVP_PIR_CONNECTED_H_
#define DEVP_PIR_CONNECTED_H_

#include <stdint.h>

void devp_pir_connected_init (int8_t enabled);

void devp_pir_connected_set (int8_t pir);

#endif//DEVP_PIR_CONNECTED_H_
