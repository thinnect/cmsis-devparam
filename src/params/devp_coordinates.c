/**
 * DeviceParamaters for node coordinates.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_coordinates.h"

#include "node_coordinates.h"

static char    m_geo_fix_type = 'U';
static int32_t m_geo_latitude = 0;
static int32_t m_geo_longitude = 0;
static int32_t m_elevation = 0;

// geo_fix_type ----------------------------------------------------------------
static int dp_geo_fix_type_get(devp_t * param, void * value);
static int dp_geo_fix_type_set(devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_geo_fix_type = {
	.name = "geo_fix_type",
	.type = DP_TYPE_STRING,
	.size = sizeof(char),
	.persist = true,
	.getf = dp_geo_fix_type_get,
	.setf = dp_geo_fix_type_set
};

static int dp_geo_fix_type_get(devp_t * param, void * value)
{
	*((char*)value) = m_geo_fix_type;
	return sizeof(char);
}

static int dp_geo_fix_type_set(devp_t * param, bool init, void * value, uint8_t size)
{
	m_geo_fix_type = *((char*)value);
	return 0;
}
// -----------------------------------------------------------------------------

// geo_latitude ----------------------------------------------------------------
static int dp_geo_latitude_get(devp_t * param, void * value);
static int dp_geo_latitude_set(devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_geo_latitude = {
	.name = "geo_latitude",
	.type = DP_TYPE_INT32,
	.size = sizeof(int32_t),
	.persist = true,
	.getf = dp_geo_latitude_get,
	.setf = dp_geo_latitude_set
};

static int dp_geo_latitude_get(devp_t * param, void * value)
{
	*((int32_t*)value) = m_geo_latitude;
	return sizeof(int32_t);
}

static int dp_geo_latitude_set(devp_t * param, bool init, void * value, uint8_t size)
{
	m_geo_latitude = *((int32_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------

// geo_longitude ---------------------------------------------------------------
static int dp_geo_longitude_get(devp_t * param, void * value);
static int dp_geo_longitude_set(devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_geo_longitude = {
	.name = "geo_longitude",
	.type = DP_TYPE_INT32,
	.size = sizeof(int32_t),
	.persist = true,
	.getf = dp_geo_longitude_get,
	.setf = dp_geo_longitude_set
};

static int dp_geo_longitude_get(devp_t * param, void * value)
{
	*((int32_t*)value) = m_geo_longitude;
	return sizeof(int32_t);
}

static int dp_geo_longitude_set(devp_t * param, bool init, void * value, uint8_t size)
{
	m_geo_longitude = *((int32_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------

// elevation -------------------------------------------------------------------
static int dp_elevation_get(devp_t * param, void * value);
static int dp_elevation_set(devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_elevation = {
	.name = "elevation",
	.type = DP_TYPE_INT32,
	.size = sizeof(int32_t),
	.persist = true,
	.getf = dp_elevation_get,
	.setf = dp_elevation_set
};

static int dp_elevation_get(devp_t * param, void * value)
{
	*((int32_t*)value) = m_elevation;
	return sizeof(int32_t);
}

static int dp_elevation_set(devp_t * param, bool init, void * value, uint8_t size)
{
	m_elevation = *((int32_t*)value);
	return 0;
}
// -----------------------------------------------------------------------------

void devp_coordinates_init()
{
	devp_register(&m_dp_geo_fix_type);
	devp_register(&m_dp_geo_latitude);
	devp_register(&m_dp_geo_longitude);
	devp_register(&m_dp_elevation);
}

bool node_coordinates_get(coordinates_geo_t * geo)
{
	if ('U' != m_geo_fix_type)
	{
		geo->latitude = m_geo_latitude;
		geo->longitude = m_geo_longitude;
		geo->elevation = m_elevation;
		geo->type = m_geo_fix_type;
		return true;
	}
	return false;
}
