/**
 * DS18B20 one point calibration parameters. Offsets to be applied to sensor values.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_ds18b20_params.h"

#include "devp.h"

static int16_t m_ds18b20_offset; // degC * 10

// Temperature in degrees * 10 -------------------------------------------------
static int dp_ds18b20_offset_get (devp_t * param, void * value);
static int dp_ds18b20_offset_set (devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_ds18b20_offset = {
	.name = "ds18b20_offset",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = true,
	.getf = dp_ds18b20_offset_get,
	.setf = dp_ds18b20_offset_set
};

static int dp_ds18b20_offset_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_ds18b20_offset;
	return sizeof(int16_t);
}

static int dp_ds18b20_offset_set (devp_t * param, bool init, void * value, uint8_t size)
{
	m_ds18b20_offset = *((int16_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------

int16_t ds18b20_temp_offset ()
{
	return m_ds18b20_offset;
}

void devp_ds18b20_params_init()
{
	devp_register(&m_dp_ds18b20_offset);
}
