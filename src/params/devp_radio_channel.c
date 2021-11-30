/**
 * DeviceParamater radio channel management options.
 *
 * Four operating scenarios:
 * 1) radio_ch_default and radio_ch_current
 *    Change radio_ch_default and reboot node to change channel.
 * 2) radio_ch_default, radio_ch_current, radio_channel.
 *    Change radio_channel to temporarily change channel - radio_ch_current
 *    will change once channel has actually been changed. Depends on the
 *    change event listener always performing the change at some point!
 *    Same as 1 for persistent changes.
 * 3) radio_channel
 *    Changing radio_channel notifies the system that the channel should be
 *    changed.
 * 4) radio_channel + DEVP_RADIO_CHANNEL_PERSISTENT
 *    Changing radio_channel notifies the system that the channel should be
 *    changed and saves the value in flash. Note that the value may be saved in
 *    flash before the channel is actually changed.
 *
 * Options 2-4 all have the issue that when performing the channel change over
 * the same radio interface, the response may be sent on the new channel, so
 * it is a bit difficult to do the change in real environments. They can,
 * however, work really well when the channel change is performed over
 * a different communications channel (UART). Currently the options also
 * depend on the change always being possible - a failure notification
 * is not implemented.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_radio_channel.h"

#include <string.h>

#include "loglevels.h"
#define __MODUUL__ "drchnl"
#define __LOG_LEVEL__ (LOG_LEVEL_devp_radio_channel & BASE_LOG_LEVEL)
#include "log.h"

static devp_changed_cb_f m_changed_cb = NULL;
static void * m_changed_user = NULL;

#ifndef DEVP_RADIO_CHANNEL_ONLY
//------------------------------------------------------------------------------
static int dp_radio_ch_default_get(devp_t * param, void * value);
static int dp_radio_ch_default_set(devp_t * param, bool init, const void * value, uint8_t size);
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

static int dp_radio_ch_default_set(devp_t * param, bool init, const void * value, uint8_t size)
{
	uint8_t ov = m_radio_ch_default;
	m_radio_ch_default = *((uint8_t*)value);
	if ((false == init)&&(ov != m_radio_ch_default)&&(NULL != m_changed_cb))
	{
		m_changed_cb(param->name, m_changed_user); // TODO Suspect this should be deferred?
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
static get_radio_channel_f mf_get_current = NULL;

static int dp_radio_ch_current_get(devp_t * param, void * value)
{
	if (NULL != mf_get_current)
	{
		*((uint8_t*)value) = mf_get_current();
		return sizeof(uint8_t);
	}
	return DEVP_UNKNOWN;
}
//------------------------------------------------------------------------------
#endif//DEVP_RADIO_CHANNEL_ONLY

static uint8_t m_radio_channel = 0;

uint8_t get_radio_channel()
{
	return m_radio_channel;
}

#if !defined(DEVP_RADIO_CHANNEL_REQUIRE_REBOOT) || defined(DEVP_RADIO_CHANNEL_PERSISTENT)
//------------------------------------------------------------------------------
static int dp_radio_channel_get(devp_t * param, void * value);
static int dp_radio_channel_set(devp_t * param, bool init, const void * value, uint8_t size);
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

static int dp_radio_channel_get(devp_t * param, void * value)
{
	*((uint8_t*)value) = m_radio_channel;
	return sizeof(uint8_t);
}

static int dp_radio_channel_set(devp_t * param, bool init, const void * value, uint8_t size)
{
	uint8_t ov = m_radio_channel;
	m_radio_channel = *((uint8_t*)value);
	if ((false == init)&&(ov != m_radio_channel)&&(NULL != m_changed_cb))
	{
		m_changed_cb(param->name, m_changed_user); // TODO Suspect this should be deferred?
	}
	return 0;
}
//------------------------------------------------------------------------------
#endif//!DEVP_RADIO_CHANNEL_REQUIRE_REBOOT || DEVP_RADIO_CHANNEL_PERSISTENT

void devp_radio_channel_init(get_radio_channel_f f_current, devp_changed_cb_f callback, void * user)
{
	m_changed_cb = callback;
	m_changed_user = user;
#ifndef DEVP_RADIO_CHANNEL_ONLY
	mf_get_current = f_current;

	devp_register(&m_dp_radio_ch_default);
	if (0 == m_radio_ch_default) // Nothing was loaded from storage
	{
		m_radio_ch_default = DEFAULT_RADIO_CHANNEL;
		int r = devp_store(m_dp_radio_ch_default.name, m_dp_radio_ch_default.type, &m_radio_ch_default, sizeof(uint8_t));
		if (sizeof(uint8_t) == r)
		{
			debug1("dflt ch %d", (int)DEFAULT_RADIO_CHANNEL);
		}
		else
		{
			err1("s=%d", r);
		}
	}

	if (0 == m_radio_channel)
	{
		m_radio_channel = m_radio_ch_default; // default would have been loaded from storage
	}

	devp_register(&m_dp_radio_ch_current);
#endif//DEVP_RADIO_CHANNEL_ONLY

#if !defined(DEVP_RADIO_CHANNEL_REQUIRE_REBOOT) || defined(DEVP_RADIO_CHANNEL_PERSISTENT)
	devp_register(&m_dp_radio_channel);
#endif//DEVP_RADIO_CHANNEL_REQUIRE_REBOOT

	if (0 == m_radio_channel) // Final fallback
	{
		m_radio_channel = DEFAULT_RADIO_CHANNEL;
	}
}
