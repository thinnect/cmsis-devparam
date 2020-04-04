/**
 * Device lifetime info through DeviceParameters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_lifetime.h"

#include "cmsis_os2_ext.h"

static int dp_lifetime_get(devp_t * param, void * value);
static int dp_lifetime_set(devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_lifetime = {
	.name = "lifetime",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = true,
	.getf = dp_lifetime_get,
	.setf = dp_lifetime_set
};

uint32_t m_lifetime_value;

static int dp_lifetime_get(devp_t * param, void * value)
{
	*((uint32_t*)value) = m_lifetime_value + osCounterGetSecond();
	return sizeof(uint32_t);
}

static int dp_lifetime_set(devp_t * param, bool init, void * value, uint8_t size)
{
	m_lifetime_value = *((uint32_t*)value);
	return 0;
}

void devp_lifetime_init()
{
	// Register will call dp_lifetime_set before it returns, if it has a stored value
	devp_register(&m_dp_lifetime);
}
