/**
 * Generic sensing retry parameter.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_sens_retry.h"

static int dp_sens_retry_get(devp_t * param, void * value);
static int dp_sens_retry_set(devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_sens_retry = {
	.name = "sens_retry",
	.type = DP_TYPE_UINT16,
	.size = sizeof(uint16_t),
	.persist = true,
	.getf = dp_sens_retry_get,
	.setf = dp_sens_retry_set
};

static uint16_t m_sens_retry;

static int dp_sens_retry_get(devp_t * param, void * value)
{
	*((uint16_t*)value) = m_sens_retry;
	return sizeof(uint16_t);
}

static int dp_sens_retry_set(devp_t * param, bool init, void * value, uint8_t size)
{
	m_sens_retry = *((uint16_t*)value);
	return 0;
}

void devp_sens_retry_init(uint16_t default_retry)
{
	m_sens_retry = default_retry;
	devp_register(&m_dp_sens_retry);
}

uint16_t devp_sens_retry_get()
{
	return m_sens_retry;
}
