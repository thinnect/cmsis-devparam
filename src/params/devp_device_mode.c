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

#include "device_modes.h" // Actual device modes defined in the header

static const char * m_device_mode_strings[] = { DEVICE_MODE_STRINGS };

static devp_changed_cb_f m_changed_cb = NULL;
static void * m_changed_user = NULL;

static int dp_default_mode_get (devp_t * param, void * value);
static int dp_default_mode_set (devp_t * param, bool init, const void * value, uint8_t size);
static devp_t m_dp_default_mode = {
	.name = "default_mode",
	.type = DP_TYPE_STRING,
	.size = DEVICE_MODE_STRINGS_MAX_LENGTH,
	.persist = true,
	.getf = dp_default_mode_get,
	.setf = dp_default_mode_set
};
static uint8_t m_default_mode = DEFAULT_DEVICE_MODE;

static int dp_default_mode_get (devp_t * param, void * value)
{
	int len = strlen(m_device_mode_strings[m_default_mode]);
	memcpy(value, m_device_mode_strings[m_default_mode], len);
	return len;
}

static int dp_default_mode_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	for (uint8_t i=0;i<DEVICE_MODE_TOTAL_COUNT;i++)
	{
		if (strlen(m_device_mode_strings[i]) == size)
		{
			if (0 == memcmp(m_device_mode_strings[i], value, size))
			{
				m_default_mode = i;
				return 0;
			}
		}
	}
	return DEVP_UNKNOWN;
}

static int dp_mode_get (devp_t * param, void * value);
static int dp_mode_set (devp_t * param, bool init, const void * value, uint8_t size);
static devp_t m_dp_mode = {
	.name = "mode",
	.type = DP_TYPE_STRING,
	.size = DEVICE_MODE_STRINGS_MAX_LENGTH,
	.persist = false,
	.getf = dp_mode_get,
	.setf = dp_mode_set
};
static uint8_t m_mode = 0;

static int dp_mode_get (devp_t * param, void * value)
{
	if (m_mode < DEVICE_MODE_TOTAL_COUNT)
	{
		int len = strlen(m_device_mode_strings[m_mode]);
		memcpy(value, m_device_mode_strings[m_mode], len);
		return len;
	}
	return DEVP_EVALUE;
}

static int dp_mode_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	bool ok = false;;

	for (uint8_t i=0;i<DEVICE_MODE_TOTAL_COUNT;i++)
	{
		if (strlen(m_device_mode_strings[i]) == size)
		{
			if (0 == memcmp(m_device_mode_strings[i], value, size))
			{
				if(i == m_mode)
				{
					return 0;
				}
				else
				{
					m_mode = i;
					ok = true;
				}
				break;
			}
		}
	}

	if (ok)
	{
		if (NULL != m_changed_cb)
		{
			m_changed_cb(param->name, m_changed_user); // TODO Suspect this should be deferred?
		}
		return 0;
	}
	return DEVP_UNKNOWN;
}

void devp_device_mode_init (devp_changed_cb_f callback, void * user)
{
	m_changed_cb = callback;
	m_changed_user = user;
	devp_register(&m_dp_default_mode);
	m_mode = m_default_mode;
	devp_register(&m_dp_mode);
}

uint8_t get_device_mode ()
{
	return m_mode;
}

void set_device_mode (uint8_t mode)
{
	m_mode = mode;
}

const char * get_device_mode_string ()
{
	return m_device_mode_strings[m_mode];
}
