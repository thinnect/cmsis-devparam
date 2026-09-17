/**
 * HDC2080 one point calibration and sensing parameters.
 *
 * Copyright Thinnect Inc. 2026
 * @license MIT
 */
#include "devp_hdc2080_params.h"

#include "devp.h"

static int16_t  m_hdc_temp_offset;   // degC * 10
static int16_t  m_hdc_rh_offset;     // % * 10


// Temperature in degrees * 10 -------------------------------------------------
static int dp_hdc_temp_offset_get (devp_t * param, void * value);
static int dp_hdc_temp_offset_set (devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_hdc_temp_offset = {
	.name = "hdc_temp_offset",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = true,
	.getf = dp_hdc_temp_offset_get,
	.setf = dp_hdc_temp_offset_set
};

static int dp_hdc_temp_offset_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_hdc_temp_offset;
	return sizeof(int16_t);
}

static int dp_hdc_temp_offset_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_hdc_temp_offset = *((int16_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------


// Relative humidity in % * 10 -------------------------------------------------
static int dp_hdc_rh_offset_get (devp_t * param, void * value);
static int dp_hdc_rh_offset_set (devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_hdc_rh_offset = {
	.name = "hdc_rh_offset",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = true,
	.getf = dp_hdc_rh_offset_get,
	.setf = dp_hdc_rh_offset_set
};

static int dp_hdc_rh_offset_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_hdc_rh_offset;
	return sizeof(int16_t);
}

static int dp_hdc_rh_offset_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_hdc_rh_offset = *((int16_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------

int16_t hdc2080_temp_offset ()
{
	return m_hdc_temp_offset;
}

int16_t hdc2080_rh_offset ()
{
	return m_hdc_rh_offset;
}

void devp_hdc2080_params_init ()
{
	devp_register(&m_dp_hdc_temp_offset);
	devp_register(&m_dp_hdc_rh_offset);
}
