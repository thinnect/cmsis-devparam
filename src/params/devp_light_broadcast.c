/**
 * DeviceParamater relative light sensor broadcast/resource behaviour control.
 *
 * Copyright Thinnect Inc. 2026
 * @license MIT
 */
#include "devp_light_broadcast.h"

#include <string.h>

#include "devp.h"

static bool m_light_broadcast;

static int dp_light_broadcast_get (devp_t * param, void * value);
static int dp_light_broadcast_set (devp_t * param, bool init, const void * value, uint8_t size);
static devp_t m_dp_light_broadcast = {
	.name = "light_broadcast",
	.type = DP_TYPE_BOOL,
	.size = sizeof(bool),
	.persist = true,
	.getf = dp_light_broadcast_get,
	.setf = dp_light_broadcast_set
};

static int dp_light_broadcast_get (devp_t * param, void * value)
{
	*((bool*)value) = m_light_broadcast;
	return sizeof(bool);
}

static int dp_light_broadcast_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_light_broadcast = *((bool*)value);
	return 0;
}

bool devp_light_broadcast_get (void)
{
	return m_light_broadcast;
}

void devp_light_broadcast_init (bool default_value)
{
	m_light_broadcast = default_value;
	devp_register(&m_dp_light_broadcast);
}
