/**
 * Platform LED usage control through DeviceParamaters.
 * When usage is 0 then LEDs are not used in application
 * 
 * Copyright Thinnect Inc. 2024
 * @license MIT
 */

#include <string.h>
#include <stdbool.h>

#include "devp_leds_usage.h"
#include "devp.h"
#include "platform.h"

static int dp_leds_usage_get(devp_t* param, void* value);
static int dp_leds_usage_set(devp_t* param, bool init, const void* value, uint8_t size);

static devp_t m_dp_leds_usage =
{
	.name = "leds_usage",
	.type = DP_TYPE_BOOL,
	.size = sizeof(bool),
	.persist = true,
	.getf = dp_leds_usage_get,
	.setf = dp_leds_usage_set
};

static bool m_leds_usage;

static int dp_leds_usage_get (devp_t* param, void* value)
{
	memcpy(value, &m_leds_usage, sizeof(m_leds_usage));
	return sizeof(m_leds_usage);
}

static int dp_leds_usage_set (devp_t* param, bool init, const void* value, uint8_t size)
{
	if (sizeof(m_leds_usage) == size)
	{
		m_leds_usage = *((bool*)value);
		return 0;
	}
	return DEVP_ESIZE;
}

bool devp_leds_usage_get (void)
{
	return m_leds_usage;
}

void devp_leds_usage_init (bool usage)
{
	m_leds_usage = usage;
	devp_register(&m_dp_leds_usage);
}
