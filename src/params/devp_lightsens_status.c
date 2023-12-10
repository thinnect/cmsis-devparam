/**
 * DeviceParamater light sensor status.
 *
 * Copyright Thinnect Inc. 2020
 * @license <PROPRIETARY>
 */
#include "devp_lightsens_status.h"

#include <string.h>

#include "devp.h"

//------------------------------------------------------------------------------
static enum DPLightsensStatus m_lightsens_status = LIGHT_SENSOR_UNINITIALIZED;

static int dp_lightsens_status_get (devp_t * param, void * value);
static int dp_lightsens_status_set (devp_t * param, bool init, const void * value, uint8_t size);
static devp_t m_dp_lightsens_status = {
	.name = "lightsens_status",
	.type = DP_TYPE_INT8,
	.size = sizeof(int8_t),
	.persist = true,
	.getf = dp_lightsens_status_get,
	.setf = dp_lightsens_status_set
};

static int dp_lightsens_status_get (devp_t * param, void * value)
{
	*((int8_t*)value) = m_lightsens_status;
	return sizeof(int8_t);
}

static int dp_lightsens_status_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	enum DPLightsensStatus new_value = (enum DPLightsensStatus)(*((uint8_t*)value));
	switch (new_value)
	{
		case LIGHT_SENSOR_DISABLED:
		case LIGHT_SENSOR_UNINITIALIZED:
			break;
		case LIGHT_SENSOR_ENABLED:
			new_value = LIGHT_SENSOR_UNINITIALIZED; // Cannot be forced to ENABLED state, it must get detected
			break;
		default:
			return DEVP_EVALUE;
	}
	m_lightsens_status = new_value;
	return 0;
}

enum DPLightsensStatus devp_lightsens_status_get (void)
{
	return m_lightsens_status;
}

void devp_lightsens_status_set (enum DPLightsensStatus status)
{
#if 0
	if (status != m_lightsens_status)
	{
		int8_t value = status;
		devp_set("lightsens_status", DP_TYPE_INT8, &value, sizeof(value));
	}
#endif
	m_lightsens_status = status;
}
//------------------------------------------------------------------------------

void devp_lightsens_status_init (void)
{
	devp_register(&m_dp_lightsens_status);
}
