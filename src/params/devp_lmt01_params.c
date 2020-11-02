/**
 * lmt01 one point calibration parameters. Offsets to be applied to sensor values.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_lmt01_params.h"

#include "devp.h"

static int16_t m_lmt01_offset; // degC * 10

// Temperature in degrees * 10 -------------------------------------------------
static int dp_lmt01_offset_get (devp_t * param, void * value);
static int dp_lmt01_offset_set (devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_lmt01_offset = {
	.name = "lmt01_offset",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = true,
	.getf = dp_lmt01_offset_get,
	.setf = dp_lmt01_offset_set
};

static int dp_lmt01_offset_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_lmt01_offset;
	return sizeof(int16_t);
}

static int dp_lmt01_offset_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_lmt01_offset = *((int16_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------

int16_t lmt01_temp_offset ()
{
	return m_lmt01_offset;
}

void devp_lmt01_params_init()
{
	devp_register(&m_dp_lmt01_offset);
}
