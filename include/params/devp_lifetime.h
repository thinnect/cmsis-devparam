/**
 * Device lifetime info through DeviceParameters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_LIFETIME_H_
#define DEVP_LIFETIME_H_

#include "devp.h"

#include "node_lifetime.h"

void devp_lifetime_init(void);

void devp_lifetime_boots_override (uint32_t boots);

#endif//DEVP_LIFETIME_H_
