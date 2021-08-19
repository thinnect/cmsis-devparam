/**
 * SCD30/4x default value for automatic self calibration.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#include "devp_scd_default_asc.h"

static uint8_t m_scd_default_asc;

static int dp_scd_default_asc_get (devp_t * param, void * value);
static int dp_scd_default_asc_set (devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_scd_default_asc = {
	.name = "scd_default_asc",
	.type = DP_TYPE_UINT8,
	.size = sizeof(uint8_t),
	.persist = true,
	.getf = dp_scd_default_asc_get,
	.setf = dp_scd_default_asc_set
};

static int dp_scd_default_asc_get (devp_t * param, void * value)
{
	*((uint8_t*)value) = m_scd_default_asc;
	return sizeof(uint8_t);
}

static int dp_scd_default_asc_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_scd_default_asc = *((uint8_t*)value);
	return sizeof(uint8_t);
}

void devp_scd_default_asc_init ()
{
    m_scd_default_asc = 1;
    devp_register(&m_dp_scd_default_asc);
}

uint8_t devp_scd_default_asc_get ()
{
    return m_scd_default_asc;
}
