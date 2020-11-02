/**
 * DeviceParamater device network mode selection options.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include <string.h>

#include "devp_device_nw_mode.h"

static const char * device_nw_mode_strings[] = {
	"mh",
	"sh"
};

#define DEVP_DEVICE_NW_MODE_STRLEN 2

static int dp_default_nw_mode_get(devp_t * param, void * value);
static int dp_default_nw_mode_set(devp_t * param, bool init, void * value, uint8_t size);
static devp_t m_dp_default_nw_mode = {
	.name = "default_nw_mode",
	.type = DP_TYPE_STRING,
	.size = DEVP_DEVICE_NW_MODE_STRLEN,
	.persist = true,
	.getf = dp_default_nw_mode_get,
	.setf = dp_default_nw_mode_set
};
static device_nw_modes_t m_default_nw_mode = DEVICE_NW_MODE_MH;

static int dp_default_nw_mode_get(devp_t * param, void * value)
{
	int len = strlen(device_nw_mode_strings[m_default_nw_mode]);
	memcpy(value, device_nw_mode_strings[m_default_nw_mode], len);
	return len;
}

static int dp_default_nw_mode_set(devp_t * param, bool init, void * value, uint8_t size)
{
	for (uint8_t i=0;i<DEVICE_NW_MODE_TOTAL_COUNT;i++)
	{
		if (strlen(device_nw_mode_strings[i]) == size)
		{
			if (0 == memcmp(device_nw_mode_strings[i], value, size))
			{
				m_default_nw_mode = i;
				return 0;
			}
		}
	}
	return DEVP_UNKNOWN;
}

static int dp_nw_mode_get(devp_t * param, void * value);
static devp_t m_dp_nw_mode = {
	.name = "nw_mode",
	.type = DP_TYPE_STRING,
	.size = DEVP_DEVICE_NW_MODE_STRLEN,
	.persist = false,
	.getf = dp_nw_mode_get,
	.setf = NULL
};
static device_nw_modes_t m_nw_mode = 0;

static int dp_nw_mode_get(devp_t * param, void * value)
{
	int len = strlen(device_nw_mode_strings[m_nw_mode]);
	memcpy(value, device_nw_mode_strings[m_nw_mode], len);
	return len;
}

void devp_device_nw_mode_init ()
{
	devp_register(&m_dp_default_nw_mode);
	m_nw_mode = m_default_nw_mode;
	devp_register(&m_dp_nw_mode);
}

device_nw_modes_t get_device_nw_mode()
{
	return m_nw_mode;
}
