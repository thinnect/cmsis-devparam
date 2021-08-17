/**
 * Platform LED control through DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#include "devp_leds.h"

#include "devp.h"

#include "platform.h"

#include <string.h>

static int dp_leds_get(devp_t * param, void * value);
static int dp_leds_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_leds = {
	.name = "leds",
	.type = DP_TYPE_UINT8,
	.size = sizeof(uint8_t),
	.persist = false,
	.getf = dp_leds_get,
	.setf = dp_leds_set
};

static int dp_leds_get (devp_t * param, void * value)
{
	uint8_t leds = PLATFORM_LedsGet();
	memcpy(value, &leds, sizeof(leds));
	return sizeof(leds);
}

static int dp_leds_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	if (sizeof(uint8_t) == size)
	{
		PLATFORM_LedsSet(*((uint8_t*)value));
		return 0;
	}
	return DEVP_ESIZE;
}

void devp_leds_init ()
{
	devp_register(&m_dp_leds);
}
