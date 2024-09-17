/**
 * DeviceParamater absolute light sensor broadcast/resource behaviour control.
 *
 * Copyright Thinnect Inc. 2023
 * @license MIT
 */
#include "devp_lux_broadcast.h"

#include <string.h>

#include "devp.h"

static bool m_lux_broadcast;

static int dp_lux_broadcast_get (devp_t * param, void * value);
static int dp_lux_broadcast_set (devp_t * param, bool init, const void * value, uint8_t size);
static devp_t m_dp_lux_broadcast = {
	.name = "lux_broadcast",
	.type = DP_TYPE_BOOL,
	.size = sizeof(bool),
	.persist = true,
	.getf = dp_lux_broadcast_get,
	.setf = dp_lux_broadcast_set
};

static int dp_lux_broadcast_get (devp_t * param, void * value)
{
	*((bool*)value) = m_lux_broadcast;
	return sizeof(bool);
}

static int dp_lux_broadcast_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_lux_broadcast = *((bool*)value);
	return 0;
}

bool devp_lux_broadcast_get (void)
{
	return m_lux_broadcast;
}

void devp_lux_broadcast_init (bool default_value)
{
	m_lux_broadcast = default_value;
	devp_register(&m_dp_lux_broadcast);
}
