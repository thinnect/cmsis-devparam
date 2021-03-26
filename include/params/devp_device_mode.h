/**
 * DeviceParamater device mode selection options.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_DEVICE_MODE_H_
#define DEVP_DEVICE_MODE_H_

#include "devp.h"

#include "device_modes.h"

void devp_device_mode_init (devp_changed_cb_f callback, void * user);

uint8_t get_device_mode ();

void set_device_mode (uint8_t mode);

const char * get_device_mode_string ();

#endif//DEVP_DEVICE_MODE_H_
