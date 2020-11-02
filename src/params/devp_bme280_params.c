/**
 * BME280 one point calibration parameters. Offsets to be applied to sensor values.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_bme280_params.h"

#include "devp.h"

static int16_t m_bme_temp_offset;     // degC * 10
static int16_t m_bme_rh_offset;       // % * 10
static int16_t m_bme_pressure_offset; // Pa


// Temperature in degrees * 10 -------------------------------------------------
static int dp_bme_temp_offset_get (devp_t * param, void * value);
static int dp_bme_temp_offset_set (devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_bme_temp_offset = {
	.name = "bme_temp_offset",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = true,
	.getf = dp_bme_temp_offset_get,
	.setf = dp_bme_temp_offset_set
};

static int dp_bme_temp_offset_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_bme_temp_offset;
	return sizeof(int16_t);
}

static int dp_bme_temp_offset_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_bme_temp_offset = *((int16_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------


// Relative humidity in % * 10 -------------------------------------------------
static int dp_bme_rh_offset_get (devp_t * param, void * value);
static int dp_bme_rh_offset_set (devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_bme_rh_offset = {
	.name = "bme_rh_offset",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = true,
	.getf = dp_bme_rh_offset_get,
	.setf = dp_bme_rh_offset_set
};

static int dp_bme_rh_offset_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_bme_rh_offset;
	return sizeof(int16_t);
}

static int dp_bme_rh_offset_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_bme_rh_offset = *((int16_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------


// Atmospheric pressure in Pascal ----------------------------------------------
static int dp_bme_pressure_offset_get (devp_t * param, void * value);
static int dp_bme_pressure_offset_set (devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_bme_pressure_offset = {
	.name = "bme_press_offset",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = true,
	.getf = dp_bme_pressure_offset_get,
	.setf = dp_bme_pressure_offset_set
};

static int dp_bme_pressure_offset_get (devp_t * param, void * value)
{
	*((int16_t*)value) = m_bme_pressure_offset;
	return sizeof(int16_t);
}

static int dp_bme_pressure_offset_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_bme_pressure_offset = *((int16_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------


int16_t bme280_temp_offset ()
{
	return m_bme_temp_offset;
}

int16_t bme280_rh_offset ()
{
	return m_bme_rh_offset;
}

int16_t bme280_pressure_offset ()
{
	return m_bme_pressure_offset;
}

void devp_bme280_params_init()
{
	devp_register(&m_dp_bme_temp_offset);
	devp_register(&m_dp_bme_rh_offset);
	devp_register(&m_dp_bme_pressure_offset);
}
