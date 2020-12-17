/**
 * Generic "output" get/set through DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_output.h"

#include "devp.h"

static int dp_output_get(devp_t * param, void * value);
static int dp_output_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_output = {
	.name = "output",
	.type = DP_TYPE_INT8,
	.size = sizeof(int8_t),
	.persist = false,
	.getf = dp_output_get,
	.setf = dp_output_set
};

static int8_t m_output;

static int dp_output_get (devp_t * param, void * value)
{
	*((int8_t*)value) = m_output;
	return sizeof(int8_t);
}

static int dp_output_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	if (sizeof(int8_t) == size)
	{
		m_output = *((int8_t*)value);
		return 0;
	}
	return DEVP_ESIZE;
}

void devp_output_init (int8_t default_value)
{
	m_output = default_value;
	devp_register(&m_dp_output);
}

int8_t devp_output_get ()
{
	return m_output;
}

void devp_output_set (int8_t output)
{
	m_output = output;
}
