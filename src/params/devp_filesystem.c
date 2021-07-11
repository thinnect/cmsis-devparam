/**
 * Get filesystem info through DeviceParameters.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#include "devp_filesystem.h"

#include "devp.h"

#include "fs.h"

#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>

static int dp_fs0_space_get(devp_t * param, void * value);

// 4294967295/4294967295

static devp_t m_dp_fs0_space = {
	.name = "fs0_space",
	.type = DP_TYPE_STRING,
	.size = 22, // 2*10 + '/'' + '\0'
	.persist = false,
	.getf = dp_fs0_space_get,
	.setf = NULL
};

static int dp_fs0_space_get (devp_t * param, void * value)
{
	uint32_t total;
	uint32_t used;
	int32_t ret = fs_info (0, &total, &used);
	if (0 == ret)
	{
		return sprintf(value, "%"PRIu32"/%"PRIu32, used, total);
	}
	else
	{
		return sprintf(value, "ERR: %"PRIi32, ret);
	}
}

void devp_filesystem_init ()
{
	devp_register(&m_dp_fs0_space);
}
