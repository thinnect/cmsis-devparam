
#include "lifetime.h"
#include "devp.h"

static int dp_lifetime_get(devp_t * param, void * value);
static int dp_lifetime_set(devp_t * param, bool init, void * value, uint8_t size);

static devp_t m_dp_lifetime = {
	.name = "lifetime",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = true,
	.getf = dp_lifetime_get,
	.setf = dp_lifetime_set
};

uint32_t m_lifetime_value;

extern uint32_t sys_uptime();

static int dp_lifetime_get(devp_t * param, void * value)
{
	*((uint32_t*)value) = m_lifetime_value + sys_uptime();
	return sizeof(uint32_t);
}

static int dp_lifetime_set(devp_t * param, bool init, void * value, uint8_t size)
{
	m_lifetime_value = *((uint32_t*)value);
	return 0;
}

void lifetime_init()
{
	// Register will call dp_lifetime_set before it returns, if it has a stored value
	devp_register(&m_dp_lifetime);
}