/**
 * Default SM3 MW sensor speed parameters.
 *
 * Copyright Thinnect Inc. 2022
 * @license MIT
 */
#include "devp_speed_params.h"

static int dp_speed_params_get_limit (devp_t * param, void * value);
static int dp_speed_params_set_limit (devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_speed_limit = {
	.name = "speed_limit",
	.type = DP_TYPE_UINT8,
	.size = sizeof(uint8_t),
	.persist = true,
	.getf = dp_speed_params_get_limit,
	.setf = dp_speed_params_set_limit
};

static uint8_t m_speed_limit;

int dp_speed_params_get_limit (devp_t * param, void * value)
{
	*((uint8_t*)value) = m_speed_limit;
	return sizeof(uint8_t);
}

int dp_speed_params_set_limit (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_speed_limit = *((uint8_t*)value);
	return 0;
}

void devp_speed_params_init ()
{
	m_speed_limit = DEFAULT_SPEED_LIMIT;
	devp_register(&m_dp_speed_limit);
}

uint8_t devp_speed_params_get_limit ()
{
	return m_speed_limit;
}
