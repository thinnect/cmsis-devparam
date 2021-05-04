/**
 * Light level status through deviceparamters.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#include "devp_light_level_raw.h"

#include "devp.h"

#include <stddef.h>

static int dp_light_level_raw_get(devp_t * param, void * value);

static devp_t m_dp_light_level_raw = {
	.name = "light_raw",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = false,
	.getf = dp_light_level_raw_get,
	.setf = NULL
};

static int16_t m_light_level_raw = -1;

static int dp_light_level_raw_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_light_level_raw;
	return sizeof(int16_t);
}

void devp_light_level_raw_init ()
{
	devp_register(&m_dp_light_level_raw);
}

void devp_light_level_raw_set (int16_t level)
{
	m_light_level_raw = level;
}
