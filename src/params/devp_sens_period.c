/**
 * Generic sensing period parameter.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_sens_period.h"

static int dp_sens_period_get(devp_t * param, void * value);
static int dp_sens_period_set(devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_sens_period = {
	.name = "sens_period_s",
	.type = DP_TYPE_UINT16,
	.size = sizeof(uint16_t),
	.persist = true,
	.getf = dp_sens_period_get,
	.setf = dp_sens_period_set
};

static uint16_t m_sens_period;

static int dp_sens_period_get(devp_t * param, void * value)
{
	*((uint16_t*)value) = m_sens_period;
	return sizeof(uint16_t);
}

static int dp_sens_period_set(devp_t * param, bool init, void * value, uint8_t size)
{
	m_sens_period = *((uint16_t*)value);
	return 0;
}

void devp_sens_period_init(uint16_t default_period)
{
	m_sens_period = default_period;
	devp_register(&m_dp_sens_period);
}

uint16_t devp_sens_period_get()
{
	return m_sens_period;
}
