/**
 * SCD4x offset parameters
 *
 * Copyright Thinnect Inc. 2026
 * @license MIT
 */
#include "devp_scd4x_params.h"

#include "devp.h"

static int16_t  m_scd4x_temp_offset;   // degC * 10
static int16_t  m_scd4x_rh_offset;     // % * 10


// Temperature in degrees * 10 -------------------------------------------------
static int dp_scd4x_temp_offset_get (devp_t * param, void * value);
static int dp_scd4x_temp_offset_set (devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_scd4x_temp_offset = {
	.name = "scd4x_tmp_offset",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = true,
	.getf = dp_scd4x_temp_offset_get,
	.setf = dp_scd4x_temp_offset_set
};

static int dp_scd4x_temp_offset_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_scd4x_temp_offset;
	return sizeof(int16_t);
}

static int dp_scd4x_temp_offset_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_scd4x_temp_offset = *((int16_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------


// Relative humidity in % * 10 -------------------------------------------------
static int dp_scd4x_rh_offset_get (devp_t * param, void * value);
static int dp_scd4x_rh_offset_set (devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_scd4x_rh_offset = {
	.name = "scd4x_rh_offset",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = true,
	.getf = dp_scd4x_rh_offset_get,
	.setf = dp_scd4x_rh_offset_set
};

static int dp_scd4x_rh_offset_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_scd4x_rh_offset;
	return sizeof(int16_t);
}

static int dp_scd4x_rh_offset_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_scd4x_rh_offset = *((int16_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------

int16_t scd4x_temp_offset (void)
{
	return m_scd4x_temp_offset;
}

int16_t scd4x_rh_offset (void)
{
	return m_scd4x_rh_offset;
}

void devp_scd4x_params_init (void)
{
	devp_register(&m_dp_scd4x_temp_offset);
	devp_register(&m_dp_scd4x_rh_offset);
}
