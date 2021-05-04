/**
 * Detected movement count over DeviceParameters.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#include "devp_movement_count.h"

#include "devp.h"

static int dp_movement_count_get(devp_t * param, void * value);
static int dp_movement_count_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_movement_count = {
	.name = "movement_count",
	.type = DP_TYPE_INT32,
	.size = sizeof(int32_t),
	.persist = false,
	.getf = dp_movement_count_get,
	.setf = dp_movement_count_set
};

static int32_t m_movement_count;

static int dp_movement_count_get(devp_t * param, void * value)
{
	*((int32_t*)value) = m_movement_count;
	return sizeof(int32_t);
}

static int dp_movement_count_set(devp_t * param, bool init, const void * value, uint8_t size)
{
	if (sizeof(m_movement_count) == size)
	{
		m_movement_count = *((int32_t*)value);
		return 0;
	}
	return DEVP_ESIZE;
}

void devp_movement_count_init ()
{
	devp_register(&m_dp_movement_count);
}

void devp_movement_count_increase ()
{
	m_movement_count++;
}

int32_t devp_movement_count_get ()
{
	return m_movement_count;
}
