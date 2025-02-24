/**
 * Boot monitor info through deviceparameters.
 *
 * Copyright RP 2024
 * @license MIT
 */
#include "devp_boot_monitor.h"
#include "boot_monitor.h"

#include "devp.h"

#include "endianness.h"

#include <string.h>

#include "loglevels.h"
#define __MODUUL__ "dbm"
#define __LOG_LEVEL__ (LOG_LEVEL_devp_boot_monitor & BASE_LOG_LEVEL)
#include "log.h"

//------------------------------------------------------------------------------
static int dp_boot_monitor_boots_get(devp_t * param, void * value);
static devp_t m_dp_boot_monitor_boots = {
	.name = "bm_boots",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = false,
	.getf = dp_boot_monitor_boots_get,
	.setf = NULL
};

static int dp_boot_monitor_boots_get(devp_t * param, void * value)
{
	*((uint32_t*)value) = boot_monitor_boots();
	return sizeof(uint32_t);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
static int dp_boot_monitor_short_get(devp_t * param, void * value);
static devp_t m_dp_boot_monitor_short = {
	.name = "bm_short",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = false,
	.getf = dp_boot_monitor_short_get,
	.setf = NULL
};

static int dp_boot_monitor_short_get(devp_t * param, void * value)
{
	*((uint32_t*)value) = boot_monitor_short_boots();
	return sizeof(uint32_t);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
static int dp_boot_monitor_uptime_get(devp_t * param, void * value);
static devp_t m_dp_boot_monitor_uptime = {
	.name = "bm_last_uptime",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = false,
	.getf = dp_boot_monitor_uptime_get,
	.setf = NULL
};

static int dp_boot_monitor_uptime_get(devp_t * param, void * value)
{
	*((uint32_t*)value) = boot_monitor_last_uptime();
	return sizeof(uint32_t);
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
static int dp_boot_monitor_flags_get(devp_t * param, void * value);
static int dp_boot_monitor_flags_set(devp_t * param, bool init, const void * value, uint8_t size);
static devp_t m_dp_boot_monitor_flags = {
	.name = "bm_flags",
	.type = DP_TYPE_RAW,
	.size = sizeof(uint32_t),
	.persist = false,
	.getf = dp_boot_monitor_flags_get,
	.setf = dp_boot_monitor_flags_set
};

static int dp_boot_monitor_flags_get(devp_t * param, void * value)
{
	*((uint32_t*)value) = hton32(boot_monitor_flags());
	return sizeof(uint32_t);
}

static int dp_boot_monitor_flags_set(devp_t * param, bool init, const void * value, uint8_t size)
{
	uint32_t flag = ntoh32(*((uint32_t*)value));
	flag &= ~boot_monitor_flags(); // Discard set flags
	if ((flag & (flag - 1)) == 0) // Only one new flag at a time
	{
		boot_monitor_set_flag(flag);
		return sizeof(uint32_t);
	}
	return DEVP_EINVAL;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
static int dp_boot_monitor_last_flags_get(devp_t * param, void * value);
static devp_t m_dp_boot_monitor_last_flags = {
	.name = "bm_last_flags",
	.type = DP_TYPE_RAW,
	.size = sizeof(uint32_t),
	.persist = false,
	.getf = dp_boot_monitor_last_flags_get,
	.setf = NULL
};

static int dp_boot_monitor_last_flags_get(devp_t * param, void * value)
{
	*((uint32_t*)value) = hton32(boot_monitor_last_flags());
	return sizeof(uint32_t);
}
//------------------------------------------------------------------------------

void devp_boot_monitor_init(void)
{
	devp_register(&m_dp_boot_monitor_boots);
	devp_register(&m_dp_boot_monitor_short);
	devp_register(&m_dp_boot_monitor_uptime);
	devp_register(&m_dp_boot_monitor_flags);
	devp_register(&m_dp_boot_monitor_last_flags);
}
