/**
 * DeviceParamater device network mode selection options.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_DEVICE_NW_MODE_H_
#define DEVP_DEVICE_NW_MODE_H_

#include "devp.h"

typedef enum device_nw_modes
{
	DEVICE_NW_MODE_MH,
	DEVICE_NW_MODE_SH,
	DEVICE_NW_MODE_TOTAL_COUNT
} device_nw_modes_t;

void devp_device_nw_mode_init();

device_nw_modes_t get_device_nw_mode();

#endif//DEVP_DEVICE_NW_MODE_H_
