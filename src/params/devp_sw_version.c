/**
 * Node software version through DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_sw_version.h"

#include <string.h>

static int dp_sw_version_get(devp_t * param, void * value);

static devp_t m_dp_version = {
	.name = "sw_version",
	.type = DP_TYPE_STRING,
	.size = sizeof(VERSION_STR) - 1,
	.persist = false,
	.getf = dp_sw_version_get,
	.setf = NULL
};

static int dp_sw_version_get(devp_t * param, void * value)
{
	memcpy(value, VERSION_STR, sizeof(VERSION_STR) - 1);
	return sizeof(VERSION_STR) - 1;
}

void devp_sw_version_init()
{
	devp_register(&m_dp_version);
}
