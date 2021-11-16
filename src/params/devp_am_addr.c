/**
 * DeviceParameter radio ActiveMessage address.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_am_addr.h"

#include "DeviceSignature.h"

#include "endianness.h" // manual endianness conversions because using RAW type

static int dp_am_addr_get(devp_t * param, void * value);
static int dp_am_addr_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_am_addr = {
	.name = "am_addr",
	.type = DP_TYPE_RAW,
	.size = sizeof(uint16_t),
	.persist = true,
	.getf = dp_am_addr_get,
	.setf = dp_am_addr_set
};

static uint16_t m_am_addr;

static int dp_am_addr_get (devp_t * param, void * value)
{
	*((uint16_t*)value) = hton16(m_am_addr);
	return sizeof(uint16_t);
}

static int dp_am_addr_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_am_addr = ntoh16(*((uint16_t*)value));
	return 0;
}

void devp_am_addr_init (void)
{
	m_am_addr = sigGetNodeId();
	devp_register(&m_dp_am_addr);
}

uint16_t devp_am_addr_get (void)
{
	return m_am_addr;
}
