/**
 * DeviceParameter for device announcement period.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#include "devp_annc_period_serial.h"

static int dp_annc_period_serial_get(devp_t * param, void * value);
static int dp_annc_period_serial_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_annc_period_serial = {
	.name = "annc_serial_s",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = true,
	.getf = dp_annc_period_serial_get,
	.setf = dp_annc_period_serial_set
};

static uint32_t m_annc_period_serial_s;

static int dp_annc_period_serial_get (devp_t * param, void * value)
{
	*((uint32_t*)value) = m_annc_period_serial_s;
	return sizeof(uint32_t);
}

static int dp_annc_period_serial_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_annc_period_serial_s = *((uint32_t*)value);
	return 0;
}

void devp_annc_period_serial_init (uint32_t default_period_s, uint32_t min_period_s)
{
	m_annc_period_serial_s = default_period_s;
	devp_register(&m_dp_annc_period_serial);
	if (m_annc_period_serial_s < min_period_s)
	{
		m_annc_period_serial_s = min_period_s;
	}
}

uint32_t devp_annc_period_serial_get (void)
{
	return m_annc_period_serial_s;
}
