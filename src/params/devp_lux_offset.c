/**
 * DeviceParamater absolute light sensor value correction offset.
 *
 * Copyright Thinnect Inc. 2023
 * @license MIT
 */
#include "devp_lux_offset.h"

#include <string.h>

#include "devp.h"

static int16_t m_lux_offset;

static int dp_lux_offset_get (devp_t * param, void * value);
static int dp_lux_offset_set (devp_t * param, bool init, const void * value, uint8_t size);
static devp_t m_dp_lux_offset = {
	.name = "lux_offset",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = true,
	.getf = dp_lux_offset_get,
	.setf = dp_lux_offset_set
};

static int dp_lux_offset_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_lux_offset;
	return sizeof(int16_t);
}

static int dp_lux_offset_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_lux_offset = *((int16_t*)value);
	return 0;
}

int16_t devp_lux_offset_get (void)
{
	return m_lux_offset;
}

void devp_lux_offset_init (int16_t default_value)
{
    m_lux_offset = default_value;
	devp_register(&m_dp_lux_offset);
}
