/**
 * DeviceParamater radio LowPowerListening parameters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_lpl.h"

static int dp_lpl_remote_wakeup_get(devp_t * param, void * value);
static int dp_lpl_remote_wakeup_set(devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_lpl_remote_wakeup = {
	.name = "lpl_remote_wakeup",
	.type = DP_TYPE_UINT16,
	.size = sizeof(uint16_t),
	.persist = false,
	.getf = dp_lpl_remote_wakeup_get,
	.setf = dp_lpl_remote_wakeup_set
};

static uint16_t m_lpl_remote_wakeup;

static int dp_lpl_remote_wakeup_get(devp_t * param, void * value)
{
	*((uint16_t*)value) = m_lpl_remote_wakeup;
	return sizeof(uint16_t);
}

static int dp_lpl_remote_wakeup_set(devp_t * param, bool init, void * value, uint8_t size)
{
	m_lpl_remote_wakeup = *((uint16_t*)value);
	return 0;
}

void devp_lpl_init()
{
	devp_register(&m_dp_lpl_remote_wakeup);
}

uint16_t devp_lpl_remote_wakeup_get()
{
	return m_lpl_remote_wakeup;
}
