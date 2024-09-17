/**
 * Light level status through deviceparamters.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#include "devp_light_level_lux.h"

#include "devp.h"

#include <stddef.h>

static int dp_light_level_lux_get(devp_t * param, void * value);

static devp_t m_dp_light_level_lux = {
	.name = "light_lux",
	.type = DP_TYPE_INT32,
	.size = sizeof(int32_t),
	.persist = false,
	.getf = dp_light_level_lux_get,
	.setf = NULL
};

static int32_t m_light_level_lux = -1;

static int dp_light_level_lux_get (devp_t * param, void * value)
{
	*((int32_t*)value) = m_light_level_lux;
	return sizeof(int32_t);
}

void devp_light_level_lux_init ()
{
	devp_register(&m_dp_light_level_lux);
}

void devp_light_level_lux_set (int32_t level)
{
	m_light_level_lux = level;
}
