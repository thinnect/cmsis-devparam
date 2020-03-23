/**
 * DeviceParamater radio channel selection options.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include <string.h>

#include "devp.h"

#include "devp_radio_channel.h"

static devp_changed_cb_f m_changed_cb = NULL;
static void * m_changed_user = NULL;

#ifndef DEVP_RADIO_CHANNEL_ONLY
//------------------------------------------------------------------------------
static int dp_radio_ch_default_get(devp_t * param, void * value);
static int dp_radio_ch_default_set(devp_t * param, bool init, void * value, uint8_t size);
static devp_t m_dp_radio_ch_default = {
	.name = "radio_ch_default",
	.type = DP_TYPE_UINT8,
	.size = sizeof(uint8_t),
	.persist = true,
	.getf = dp_radio_ch_default_get,
	.setf = dp_radio_ch_default_set
};
static uint8_t m_radio_ch_default = 0;

uint8_t get_radio_ch_default()
{
	return m_radio_ch_default;
}

static int dp_radio_ch_default_get(devp_t * param, void * value)
{
	*((uint8_t*)value) = m_radio_ch_default;
	return sizeof(uint8_t);
}

static int dp_radio_ch_default_set(devp_t * param, bool init, void * value, uint8_t size)
{
	uint8_t ov = m_radio_ch_default;
	m_radio_ch_default = *((uint8_t*)value);
	if (ov != m_radio_ch_default)
	{
		if (NULL != m_changed_cb)
		{
			m_changed_cb(param->name, m_changed_user); // TODO Suspect this should be deferred?
		}
	}
	return 0;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
static int dp_radio_ch_current_get(devp_t * param, void * value);
static devp_t m_dp_radio_ch_current = {
	.name = "radio_ch_current",
	.type = DP_TYPE_UINT8,
	.size = sizeof(uint8_t),
	.persist = false,
	.getf = dp_radio_ch_current_get,
	.setf = NULL
};
static get_radio_channel_f m_get_current_func = NULL;

static int dp_radio_ch_current_get(devp_t * param, void * value)
{
	if (NULL != m_get_current_func)
	{
		*((uint8_t*)value) = m_get_current_func();
	}
	return -4;
}
//------------------------------------------------------------------------------
#endif//DEVP_RADIO_CHANNEL_ONLY

//------------------------------------------------------------------------------
static int dp_radio_channel_get(devp_t * param, void * value);
static int dp_radio_channel_set(devp_t * param, bool init, void * value, uint8_t size);
static devp_t m_dp_radio_channel = {
	.name = "radio_channel",
	.type = DP_TYPE_UINT8,
	.size = sizeof(uint8_t),
#ifdef DEVP_RADIO_CHANNEL_PERSISTENT
	.persist = true,
#else
	.persist = false,
#endif//DEVP_RADIO_CHANNEL_PERSISTENT
	.getf = dp_radio_channel_get,
	.setf = dp_radio_channel_set
};
static uint8_t m_radio_channel = 0;

uint8_t get_radio_channel()
{
	return m_radio_channel;
}

static int dp_radio_channel_get(devp_t * param, void * value)
{
	*((uint8_t*)value) = m_radio_channel;
	return sizeof(uint8_t);
}

static int dp_radio_channel_set(devp_t * param, bool init, void * value, uint8_t size)
{
	uint8_t ov = m_radio_channel;
	m_radio_channel = *((uint8_t*)value);
	if (ov != m_radio_channel)
	{
		if (NULL != m_changed_cb)
		{
			m_changed_cb(param->name, m_changed_user); // TODO Suspect this should be deferred?
		}
	}
	return 0;
}
//------------------------------------------------------------------------------

void devp_radio_channel_init(devp_changed_cb_f callback, void * user)
{
	m_changed_cb = callback;
	m_changed_user = user;
#ifndef DEVP_RADIO_CHANNEL_ONLY
	devp_register(&m_dp_radio_ch_default);
	devp_register(&m_dp_radio_ch_current);
#endif//DEVP_RADIO_CHANNEL_ONLY
	devp_register(&m_dp_radio_channel);
}
