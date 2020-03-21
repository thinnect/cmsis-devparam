
#include "unity.h"

#include "devp.h"

#include "lifetime.h"
#include "uptime.h"

uint32_t sys_uptime()
{
	return 7;
}

void setUp()
{
	devp_init();
	lifetime_init();
	uptime_init();
}

void test_get()
{
	uint32_t vu32;

	TEST_ASSERT_EQUAL_INT(sizeof(vu32), devp_get("lifetime", DP_TYPE_UINT32, &vu32, sizeof(vu32)));

	TEST_ASSERT_EQUAL_INT(sizeof(vu32), devp_get("uptime", DP_TYPE_UINT32, &vu32, sizeof(vu32)));
}

void test_discover_idx()
{
	uint32_t vu32;
	const char * pname;
	DeviceParameterTypes_t tp;

	TEST_ASSERT_EQUAL_INT(sizeof(vu32), devp_discover_idx(0, &pname, &tp, &vu32, sizeof(vu32)));
	TEST_ASSERT_EQUAL_STRING("lifetime", pname);
	TEST_ASSERT_EQUAL_INT(DP_TYPE_UINT32, tp);

	TEST_ASSERT_EQUAL_INT(sizeof(vu32), devp_discover_idx(1, &pname, &tp, &vu32, sizeof(vu32)));
	TEST_ASSERT_EQUAL_STRING("uptime", pname);
	TEST_ASSERT_EQUAL_INT(DP_TYPE_UINT32, tp);
}

void test_discover_by_name()
{
	uint32_t vu32 = 0;
	DeviceParameterTypes_t tp = 0;

	TEST_ASSERT_EQUAL_INT(sizeof(vu32), devp_discover_name("lifetime", &tp, &vu32, sizeof(vu32)));
	TEST_ASSERT_EQUAL_INT(DP_TYPE_UINT32, tp);
	TEST_ASSERT_EQUAL_INT(7, vu32);

	TEST_ASSERT_EQUAL_INT(sizeof(vu32), devp_discover_name("uptime", &tp, &vu32, sizeof(vu32)));
	TEST_ASSERT_EQUAL_INT(DP_TYPE_UINT32, tp);
	TEST_ASSERT_EQUAL_INT(7, vu32);
}