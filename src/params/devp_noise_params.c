/**
 * Default noise sensor parameters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_noise_params.h"

static int dp_noise_params_get_offset (devp_t * param, void * value);
static int dp_noise_params_set_offset (devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_noise_offset = {
	.name = "noise_offset",
	.type = DP_TYPE_UINT8,
	.size = sizeof(uint8_t),
	.persist = true,
	.getf = dp_noise_params_get_offset,
	.setf = dp_noise_params_set_offset
};

static uint16_t m_noise_offset;

int dp_noise_params_get_offset (devp_t * param, void * value)
{
	*((uint8_t*)value) = m_noise_offset;
	return sizeof(uint8_t);
}

int dp_noise_params_set_offset (devp_t * param, bool init, void * value, uint8_t size)
{
	m_noise_offset = *((uint8_t*)value);
	return 0;
}

void devp_noise_params_init ()
{
	m_noise_offset = DEFAULT_NOISE_OFFSET;
	devp_register(&m_dp_noise_offset);
}

uint8_t devp_noise_params_get_offset ()
{
	return m_noise_offset;
}
