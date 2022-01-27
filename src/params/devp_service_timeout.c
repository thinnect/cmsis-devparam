/**
 * Deviceparameter parameter to configure keep alive timeout for deviceparameter.
 * Is stored in persistent memory.
 * Default value currently set to 5 sec
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#include "devp_service_timeout.h"


#ifndef DEVP_SERVICE_TIMEOUT_MS
#define DEVP_SERVICE_TIMEOUT_MS 5000
#endif//DEVP_SERVICE_TIMEOUT_MS

static uint32_t m_service_mode_timeout;

static int dp_service_mode_timeout_get (devp_t * param, void * value);
static int dp_service_mode_timeout_set (devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_service_mode_timeout = {
	.name = "service_to",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = true,
	.getf = dp_service_mode_timeout_get,
	.setf = dp_service_mode_timeout_set
};

static int dp_service_mode_timeout_get (devp_t * param, void * value)
{
	*((uint32_t*)value) = m_service_mode_timeout;
	return sizeof(uint32_t);
}

static int dp_service_mode_timeout_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_service_mode_timeout = *((uint32_t*)value);
    return sizeof(uint32_t);
}

void devp_service_mode_timeout_init ()
{
    m_service_mode_timeout = DEVP_SERVICE_TIMEOUT_MS;
    devp_register(&m_dp_service_mode_timeout);
}

uint32_t devp_service_mode_timeout_get ()
{
    return m_service_mode_timeout;
}
