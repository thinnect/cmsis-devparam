/**
 * Node software name through DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_ident_appname.h"

#include <string.h>

static int dp_ident_appname_get(devp_t * param, void * value);

static devp_t m_dp_ident_appname = {
	.name = "ident_appname",
	.type = DP_TYPE_STRING,
	.size = sizeof(IDENT_APPNAME) - 1,
	.persist = false,
	.getf = dp_ident_appname_get,
	.setf = NULL
};

static int dp_ident_appname_get(devp_t * param, void * value)
{
	memcpy(value, IDENT_APPNAME, sizeof(IDENT_APPNAME) - 1);
	return sizeof(IDENT_APPNAME) - 1;
}

void devp_ident_appname_init()
{
	devp_register(&m_dp_ident_appname);
}
