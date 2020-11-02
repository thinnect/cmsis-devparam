/**
 * Default PAN ID parameter.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_pan_id.h"

#include "endianness.h" // manual endianness conversions because using RAW type

static int dp_pan_id_get(devp_t * param, void * value);
static int dp_pan_id_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_pan_id = {
	.name = "pan_id",
	.type = DP_TYPE_RAW,
	.size = sizeof(uint16_t),
	.persist = true,
	.getf = dp_pan_id_get,
	.setf = dp_pan_id_set
};

static uint16_t m_pan_id;

static int dp_pan_id_get(devp_t * param, void * value)
{
	*((uint16_t*)value) = hton16(m_pan_id);
	return sizeof(uint16_t);
}

static int dp_pan_id_set(devp_t * param, bool init, const void * value, uint8_t size)
{
	m_pan_id = ntoh16(*((uint16_t*)value));
	return 0;
}

void devp_pan_id_init()
{
	m_pan_id = DEFAULT_PAN_ID;
	devp_register(&m_dp_pan_id);
}

uint16_t devp_pan_id_get()
{
	return m_pan_id;
}
