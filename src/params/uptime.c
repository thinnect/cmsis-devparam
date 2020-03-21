
#include <string.h>

#include "uptime.h"
#include "devp.h"

static int dp_uptime_get(devp_t * param, void * value);

static devp_t m_dp_uptime = {
	.name = "uptime",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = false,
	.getf = dp_uptime_get,
	.setf = NULL
};

extern uint32_t sys_uptime();

static int dp_uptime_get(devp_t * param, void * value)
{
	*((uint32_t*)value) = sys_uptime();
	return sizeof(uint32_t);
}

void uptime_init()
{
	devp_register(&m_dp_uptime);
}