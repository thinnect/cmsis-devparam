/**
 * DeviceParamater radio Active Message address.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_unix_time.h"

#include "DeviceSignature.h"

#include "time_rtc.h"

#include <stddef.h>

static devp_unix_time_changed_f mf_unix_time_changed;

static int dp_unix_time_get(devp_t * param, void * value);
static int dp_unix_time_set(devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_unix_time = {
	.name = "unix_time",
	.type = DP_TYPE_INT64,
	.size = sizeof(int64_t),
	.persist = false,
	.getf = dp_unix_time_get,
	.setf = dp_unix_time_set
};

static int dp_unix_time_get(devp_t * param, void * value)
{
	*((int64_t*)value) = time(NULL);
	return sizeof(int64_t);
}

static int dp_unix_time_set(devp_t * param, bool init, void * value, uint8_t size)
{
	if(sizeof(int64_t) != size)
	{
		return DEVP_ESIZE;
	}
	else
	{
		time_t t = *((int64_t*)value);
		if (0 != time_rtc_stime(&t))
		{
			return DEVP_EINVAL;
		}
		if (NULL != mf_unix_time_changed)
		{
			mf_unix_time_changed();
		}
	}
	return 0;
}

void devp_unix_time_init(devp_unix_time_changed_f changed)
{
	mf_unix_time_changed = changed;
	devp_register(&m_dp_unix_time);
}
