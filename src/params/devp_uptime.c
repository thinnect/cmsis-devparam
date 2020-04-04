/**
 * Device uptime info through DeviceParameters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_uptime.h"

#include <string.h>

#include "cmsis_os2_ext.h"

static int dp_uptime_get(devp_t * param, void * value);

static devp_t m_dp_uptime = {
	.name = "uptime",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = false,
	.getf = dp_uptime_get,
	.setf = NULL
};

static int dp_uptime_get(devp_t * param, void * value)
{
	*((uint32_t*)value) = osCounterGetSecond();
	return sizeof(uint32_t);
}

void devp_uptime_init()
{
	devp_register(&m_dp_uptime);
}
