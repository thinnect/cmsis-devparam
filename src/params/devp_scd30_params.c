/**
 * SCD30 one point calibration and sensing parameters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_scd30_params.h"

#include "devp.h"

static int16_t  m_scd_temp_offset;   // degC * 10
static int16_t  m_scd_rh_offset;     // % * 10
static int16_t  m_scd_co2_offset;    // ppm
static uint16_t m_scd_warmup_s = 60; // Seconds


// Temperature in degrees * 10 -------------------------------------------------
static int dp_scd_temp_offset_get (devp_t * param, void * value);
static int dp_scd_temp_offset_set (devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_scd_temp_offset = {
	.name = "scd_temp_offset",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = true,
	.getf = dp_scd_temp_offset_get,
	.setf = dp_scd_temp_offset_set
};

static int dp_scd_temp_offset_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_scd_temp_offset;
	return sizeof(int16_t);
}

static int dp_scd_temp_offset_set (devp_t * param, bool init, void * value, uint8_t size)
{
	m_scd_temp_offset = *((int16_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------


// Relative humidity in % * 10 -------------------------------------------------
static int dp_scd_rh_offset_get (devp_t * param, void * value);
static int dp_scd_rh_offset_set (devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_scd_rh_offset = {
	.name = "scd_rh_offset",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = true,
	.getf = dp_scd_rh_offset_get,
	.setf = dp_scd_rh_offset_set
};

static int dp_scd_rh_offset_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_scd_rh_offset;
	return sizeof(int16_t);
}

static int dp_scd_rh_offset_set (devp_t * param, bool init, void * value, uint8_t size)
{
	m_scd_rh_offset = *((int16_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------


// CO2 in ppm ------------------------------------------------------------------
static int dp_scd_co2_offset_get (devp_t * param, void * value);
static int dp_scd_co2_offset_set (devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_scd_co2_offset = {
	.name = "scd_co2_offset",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = true,
	.getf = dp_scd_co2_offset_get,
	.setf = dp_scd_co2_offset_set
};

static int dp_scd_co2_offset_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_scd_co2_offset;
	return sizeof(int16_t);
}

static int dp_scd_co2_offset_set (devp_t * param, bool init, void * value, uint8_t size)
{
	m_scd_co2_offset = *((int16_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------


// Warmup ----------------------------------------------------------------------
static int dp_scd_warmup_get (devp_t * param, void * value);
static int dp_scd_warmup_set (devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_scd_warmup = {
	.name = "scd_warmup_s",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = true,
	.getf = dp_scd_warmup_get,
	.setf = dp_scd_warmup_set
};

static int dp_scd_warmup_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_scd_warmup_s;
	return sizeof(int16_t);
}

static int dp_scd_warmup_set (devp_t * param, bool init, void * value, uint8_t size)
{
	m_scd_warmup_s = *((int16_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------


int16_t scd30_temp_offset ()
{
	return m_scd_temp_offset;
}

int16_t scd30_rh_offset ()
{
	return m_scd_rh_offset;
}

int16_t scd30_co2_offset ()
{
	return m_scd_co2_offset;
}

uint16_t scd30_warmup_get ()
{
	return m_scd_warmup_s;
}

void devp_scd30_params_init ()
{
	devp_register(&m_dp_scd_temp_offset);
	devp_register(&m_dp_scd_rh_offset);
	devp_register(&m_dp_scd_co2_offset);
	devp_register(&m_dp_scd_warmup);
}
