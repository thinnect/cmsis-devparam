/**
 * DeviceParamater device mode selection.
 *
 * The default_mode persists, mode is intended for temporary change.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include <string.h>

#include "devp_device_mode.h"

typedef enum sm_modes {
	SM_MODE_DEFAULT,
	SM_MODE_SMART,
	SM_MODE_SMART_HEATING,
	SM_MODE_TOTAL_COUNT
} sm_modes_t;

#define SM_MODE_STRINGS "default","smart","smart_heat"

// Longest string in this case is 'smart_heat'
#define SM_MODE_STRINGS_MAX_LENGTH (sizeof("smart_heat")-1)

static const char * m_sm_mode_strings[] = { SM_MODE_STRINGS };

static int dp_sm_mode_get (devp_t *param, void *value);
static int dp_sm_mode_set (devp_t *param, bool init, const void *value, uint8_t size);

static devp_t m_dp_sm_mode = {
	.name = "sm_mode",
	.type = DP_TYPE_STRING,
	.size = SM_MODE_STRINGS_MAX_LENGTH,
	.persist = true,
	.getf = dp_sm_mode_get,
	.setf = dp_sm_mode_set
};

static uint8_t m_sm_mode;

int dp_sm_mode_get (devp_t *param, void *value)
{
	if (m_sm_mode < SM_MODE_TOTAL_COUNT)
	{
		const int len = strlen(m_sm_mode_strings[m_sm_mode]);
		
		memcpy(value, m_sm_mode_strings[m_sm_mode], len);
		
		return len;
	}
	return DEVP_EVALUE;
}

int dp_sm_mode_set (devp_t *param, bool init, const void *value, uint8_t size)
{
	for (uint8_t i = 0; i < SM_MODE_TOTAL_COUNT; i++)
	{
		if (strlen(m_sm_mode_strings[i]) == size)
		{
			if (0 == memcmp(m_sm_mode_strings[i], value, size))
			{
				m_sm_mode = (sm_modes_t)i;
				
				return 0;
			}
		}
	}
	return DEVP_UNKNOWN;
}

void devp_sm_mode_init ()
{
	m_sm_mode = SM_MODE_DEFAULT;
	devp_register(&m_dp_sm_mode);
}

uint8_t get_sm_mode ()
{
	return m_sm_mode;
}

void set_sm_mode (uint8_t sm_mode)
{
	if (mode < SM_MODE_TOTAL_COUNT)
	{
		m_sm_mode = sm_mode;
	}
}

const char *get_sm_mode_string ()
{
	return m_sm_mode_strings[m_sm_mode];
}

