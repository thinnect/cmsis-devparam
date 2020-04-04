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

#if defined(DEFAULT_DEVICE_MODE_SNIFFER)
	#pragma message("DEVICE_MODE_SNIFFER")
	#define DEFAULT_DEVICE_MODE DEVICE_MODE_SNIFFER
#elif defined(DEFAULT_DEVICE_MODE_CONNECTOR)
	#pragma message("DEVICE_MODE_CONNECTOR")
	#define DEFAULT_DEVICE_MODE DEVICE_MODE_CONNECTOR
#else
	#define DEFAULT_DEVICE_MODE DEVICE_MODE_GATEWAY
#endif

const char * device_mode_strings[] = {
	"gateway",
	"sniffer",
	"connector"
};

#define DEVP_DEVICE_MODE_STRLEN (sizeof("connector")-1) // Longest string

static devp_changed_cb_f m_changed_cb = NULL;
static void * m_changed_user = NULL;

static int dp_default_mode_get(devp_t * param, void * value);
static int dp_default_mode_set(devp_t * param, bool init, void * value, uint8_t size);
static devp_t m_dp_default_mode = {
	.name = "default_mode",
	.type = DP_TYPE_STRING,
	.size = DEVP_DEVICE_MODE_STRLEN,
	.persist = true,
	.getf = dp_default_mode_get,
	.setf = dp_default_mode_set
};
static uint8_t m_default_mode = DEFAULT_DEVICE_MODE;

static int dp_default_mode_get(devp_t * param, void * value)
{
	int len = strlen(device_mode_strings[m_default_mode]);
	memcpy(value, device_mode_strings[m_default_mode], len);
	return len;
}

static int dp_default_mode_set(devp_t * param, bool init, void * value, uint8_t size)
{
	for (uint8_t i=0;i<DEVICE_MODE_TOTAL_COUNT;i++)
	{
		if (strlen(device_mode_strings[i]) == size)
		{
			if (0 == memcmp(device_mode_strings[i], value, size))
			{
				m_default_mode = i;
				return 0;
			}
		}
	}
	return DEVP_UNKNOWN;
}

static int dp_mode_get(devp_t * param, void * value);
static int dp_mode_set(devp_t * param, bool init, void * value, uint8_t size);
static devp_t m_dp_mode = {
	.name = "mode",
	.type = DP_TYPE_STRING,
	.size = DEVP_DEVICE_MODE_STRLEN,
	.persist = false,
	.getf = dp_mode_get,
	.setf = dp_mode_set
};
static uint8_t m_mode = 0;

static int dp_mode_get(devp_t * param, void * value)
{
	int len = strlen(device_mode_strings[m_mode]);
	memcpy(value, device_mode_strings[m_mode], len);
	return len;
}

static int dp_mode_set(devp_t * param, bool init, void * value, uint8_t size)
{
	bool ok = false;;

	for (uint8_t i=0;i<DEVICE_MODE_TOTAL_COUNT;i++)
	{
		if (strlen(device_mode_strings[i]) == size)
		{
			if (0 == memcmp(device_mode_strings[i], value, size))
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

void devp_device_mode_init(devp_changed_cb_f callback, void * user)
{
	m_changed_cb = callback;
	m_changed_user = user;
	devp_register(&m_dp_default_mode);
	m_mode = m_default_mode;
	devp_register(&m_dp_mode);
}

uint8_t get_device_mode()
{
	return m_mode;
}
