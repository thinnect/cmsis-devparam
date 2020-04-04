/**
 * DeviceParamater device mode selection options.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_DEVICE_MODE_H_
#define DEVP_DEVICE_MODE_H_

#include "devp.h"

enum device_modes
{
	DEVICE_MODE_GATEWAY,
	DEVICE_MODE_SNIFFER,
	DEVICE_MODE_CONNECTOR,
	DEVICE_MODE_TOTAL_COUNT
};

void devp_device_mode_init(devp_changed_cb_f callback, void * user);

uint8_t get_device_mode();

#endif//DEVP_DEVICE_MODE_H_
