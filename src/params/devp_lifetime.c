/**
 * Device lifetime seconds and boot count info through DeviceParameters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_lifetime.h"

#include "cmsis_os2_ext.h"

static int dp_lifetime_get (devp_t * param, void * value);
static int dp_lifetime_set (devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_lifetime = {
	.name = "lifetime",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = true,
	.getf = dp_lifetime_get,
	.setf = dp_lifetime_set
};

static uint32_t m_lifetime_initial;

static int dp_lifetime_get (devp_t * param, void * value)
{
	*((uint32_t*)value) = m_lifetime_initial + osCounterGetSecond();
	return sizeof(uint32_t);
}

static int dp_lifetime_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	if (init) // Only once at boot, subsequent writes have no effect to runtime values
	{
		m_lifetime_initial = *((uint32_t*)value);
		return 0;
	}
	return DEVP_EREADONLY;
}

// -----------------------------------------------------------------------------

static int dp_boots_get (devp_t * param, void * value);
static int dp_boots_set (devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_boots = {
	.name = "boots",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = true,
	.getf = dp_boots_get,
	.setf = dp_boots_set
};

static uint32_t m_lifetime_boots;
static bool m_boots_stored;

static int dp_boots_get (devp_t * param, void * value)
{
	*((uint32_t*)value) = m_lifetime_boots;
	return sizeof(uint32_t);
}

static int dp_boots_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	if (init) // Only once at boot, subsequent writes have no effect to runtime values
	{
		m_lifetime_boots = *((uint32_t*)value) + 1;
		return 0;
	}
	return DEVP_EREADONLY;
}

// -----------------------------------------------------------------------------

void devp_lifetime_init ()
{
	devp_register(&m_dp_lifetime);
	devp_register(&m_dp_boots);
	m_boots_stored = false;
}

void node_lifetime_sync ()
{
	uint32_t lifetime = node_lifetime_seconds();
	devp_store("lifetime", DP_TYPE_UINT32, &lifetime, sizeof(uint32_t));

	if (false == m_boots_stored) // Need to store only once per boot
	{
		uint32_t boots = node_lifetime_boots();
		devp_store("boots", DP_TYPE_UINT32, &boots, sizeof(uint32_t));
		m_boots_stored = true;
	}
}

uint32_t node_lifetime_seconds ()
{
	return m_lifetime_initial + osCounterGetSecond();
}

uint32_t node_lifetime_boots ()
{
	return m_lifetime_boots;
}
