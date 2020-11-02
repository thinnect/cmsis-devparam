/**
 * Reboot device by setting the reboot parameter throug DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_reboot.h"

#include "cmsis_os2_ext.h"

static int dp_reboot_get(devp_t * param, void * value);
static int dp_reboot_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_reboot = {
	.name = "reboot",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = false,
	.getf = dp_reboot_get,
	.setf = dp_reboot_set
};

static uint32_t m_reboot_base;
static uint32_t m_reboot_time;
static osTimerId_t m_reboot_timer;

static void reboot_timer_fired(void * arg)
{
	// TODO the reboot function should be called from thread context
	osKernelReboot();
}

static int dp_reboot_get(devp_t * param, void * value)
{
	if (UINT32_MAX == m_reboot_time)
	{
		*((uint32_t*)value) = UINT32_MAX;
	}
	else
	{
		*((uint32_t*)value) = (m_reboot_base + m_reboot_time) - osCounterGetMilli();
	}
	return sizeof(uint32_t);
}

static int dp_reboot_set(devp_t * param, bool init, const void * value, uint8_t size)
{
	m_reboot_base = osCounterGetMilli();
	m_reboot_time = *((uint32_t*)value);
	if(UINT32_MAX == m_reboot_time)
	{
		osTimerStop(m_reboot_timer);
	}
	else
	{
		if(m_reboot_time < 1000)
		{
			m_reboot_time = 1000;
		}
		osTimerStart(m_reboot_timer, m_reboot_time);
	}
	return 0;
}

void devp_reboot_init()
{
	m_reboot_time = UINT32_MAX;
	m_reboot_timer = osTimerNew(reboot_timer_fired, osTimerOnce, NULL, NULL);
	devp_register(&m_dp_reboot);
}
