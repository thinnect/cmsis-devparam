/**
 * Light level status through deviceparamters.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#include "devp_light_level_pct.h"

#include "devp.h"

#include <stddef.h>

static int dp_light_level_pct_get(devp_t * param, void * value);

static devp_t m_dp_light_level_pct = {
	.name = "light_pct",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = false,
	.getf = dp_light_level_pct_get,
	.setf = NULL
};

static int16_t m_light_level_pct = -1;

static int dp_light_level_pct_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_light_level_pct;
	return sizeof(int16_t);
}

void devp_light_level_pct_init ()
{
	devp_register(&m_dp_light_level_pct);
}

void devp_light_level_pct_set (int16_t level)
{
	m_light_level_pct = level;
}
