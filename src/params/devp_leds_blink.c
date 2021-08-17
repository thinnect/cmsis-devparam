/**
 * Platform LED blinking through DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#include "devp_leds.h"

#include "devp.h"

#include "platform.h"

#include "cmsis_os2.h"

#include <string.h>

static uint8_t m_blink_leds = 0;

static uint32_t m_blink_on_ms = 0; // Disabled when 0
static uint32_t m_blink_off_ms = 0; // Disabled when 0

static osTimerId_t m_blink_timer;
static osMutexId_t m_blink_mutex;


static void leds_blink_timer (void * argument)
{
	while (osOK != osMutexAcquire(m_blink_mutex, osWaitForever));

	if (0 == PLATFORM_LedsGet())
	{
		PLATFORM_LedsSet(m_blink_leds);
		if (m_blink_on_ms > 0)
		{
			osTimerStart(m_blink_timer, m_blink_on_ms);
		}
	}
	else
	{
		PLATFORM_LedsSet(0);
		if (m_blink_off_ms > 0)
		{
			osTimerStart(m_blink_timer, m_blink_off_ms);
		}
	}

	osMutexRelease(m_blink_mutex);
}


static void unguarded_configure_blink_timer ()
{
	PLATFORM_LedsSet(m_blink_leds);
	if ((m_blink_off_ms > 0)&&(m_blink_on_ms > 0))
	{
		osTimerStart(m_blink_timer, m_blink_off_ms);
	}
}


//------------------------------------------------------------------------------
static int dp_leds_blink_get(devp_t * param, void * value);
static int dp_leds_blink_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_leds_blink = {
	.name = "blink_leds",
	.type = DP_TYPE_UINT8,
	.size = sizeof(uint8_t),
	.persist = false,
	.getf = dp_leds_blink_get,
	.setf = dp_leds_blink_set
};

static int dp_leds_blink_get (devp_t * param, void * value)
{
	memcpy(value, &m_blink_leds, sizeof(m_blink_leds));
	return sizeof(m_blink_leds);
}

static int dp_leds_blink_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	if (sizeof(m_blink_leds) == size)
	{
		while (osOK != osMutexAcquire(m_blink_mutex, osWaitForever));
		memcpy(&m_blink_leds, value, size);
		unguarded_configure_blink_timer();
		osMutexRelease(m_blink_mutex);
		return 0;
	}
	return DEVP_ESIZE;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
static int dp_leds_blink_on_get(devp_t * param, void * value);
static int dp_leds_blink_on_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_leds_blink_on = {
	.name = "blink_on",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = false,
	.getf = dp_leds_blink_on_get,
	.setf = dp_leds_blink_on_set
};

static int dp_leds_blink_on_get (devp_t * param, void * value)
{
	memcpy(value, &m_blink_on_ms, sizeof(m_blink_on_ms));
	return sizeof(m_blink_on_ms);
}

static int dp_leds_blink_on_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	if (sizeof(m_blink_on_ms) == size)
	{
		while (osOK != osMutexAcquire(m_blink_mutex, osWaitForever));
		memcpy(&m_blink_on_ms, value, size);
		unguarded_configure_blink_timer();
		osMutexRelease(m_blink_mutex);
		return 0;
	}
	return DEVP_ESIZE;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
static int dp_leds_blink_off_get(devp_t * param, void * value);
static int dp_leds_blink_off_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_leds_blink_off = {
	.name = "blink_off",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = false,
	.getf = dp_leds_blink_off_get,
	.setf = dp_leds_blink_off_set
};

static int dp_leds_blink_off_get (devp_t * param, void * value)
{
	memcpy(value, &m_blink_off_ms, sizeof(m_blink_off_ms));
	return sizeof(m_blink_off_ms);
}

static int dp_leds_blink_off_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	if (sizeof(m_blink_off_ms) == size)
	{
		while (osOK != osMutexAcquire(m_blink_mutex, osWaitForever));
		memcpy(&m_blink_off_ms, value, size);
		unguarded_configure_blink_timer();
		osMutexRelease(m_blink_mutex);
		return 0;
	}
	return DEVP_ESIZE;
}
//------------------------------------------------------------------------------


void devp_leds_blink_init ()
{
	m_blink_mutex = osMutexNew(NULL);
	m_blink_timer = osTimerNew(&leds_blink_timer, osTimerOnce, NULL, NULL);
	if ((NULL != m_blink_mutex)&&(NULL != m_blink_timer))
	{
		devp_register(&m_dp_leds_blink);
		devp_register(&m_dp_leds_blink_on);
		devp_register(&m_dp_leds_blink_off);
	}
}
