/**
 * DeviceParamater IDENT_TIMESTAMP.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_ident_timestamp.h"

#include "endianness.h" // manual endianness conversions because using RAW type

static int dp_ident_timestamp_get(devp_t * param, void * value);

static devp_t m_dp_ident_timestamp = {
	.name = "ident_timestamp",
	.type = DP_TYPE_RAW,
	.size = sizeof(uint32_t),
	.persist = false,
	.getf = dp_ident_timestamp_get,
	.setf = NULL
};

static int dp_ident_timestamp_get(devp_t * param, void * value)
{
	*((uint32_t*)value) = hton32(IDENT_TIMESTAMP);
	return sizeof(uint32_t);
}

void devp_ident_timestamp_init()
{
	devp_register(&m_dp_ident_timestamp);
}
