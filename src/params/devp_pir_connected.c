/**
 * PIR hardware connection status through deviceparameters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_pir_connected.h"

#include "devp.h"

#include <stddef.h>

static int dp_pir_connected_get(devp_t * param, void * value);

static devp_t m_dp_pir_connected = {
	.name = "pir_connected",
	.type = DP_TYPE_INT8,
	.size = sizeof(int8_t),
	.persist = false,
	.getf = dp_pir_connected_get,
	.setf = NULL
};

static int8_t m_pir_connected;

static int dp_pir_connected_get (devp_t * param, void * value)
{
	*((int8_t*)value) = m_pir_connected;
	return sizeof(int8_t);
}

void devp_pir_connected_init (int8_t enabled)
{
	m_pir_connected = enabled;
	devp_register(&m_dp_pir_connected);
}

void devp_pir_connected_set (int8_t pir)
{
	m_pir_connected = pir;
}
