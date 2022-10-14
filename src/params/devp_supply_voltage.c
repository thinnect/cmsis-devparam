/**
 * @file devp_supply_voltage.c
 * @author Lembitu Valdmets
 * @brief Can access to supply voltage, uses sensors.c --> read_battery_universal for this.
 * @version 0.1
 * @date 2022-10-14
 * @license priorietary
 *
 */

#include "devp_supply_voltage.h"

#include "sensors.h"
#include "stdint.h"

static int dp_battery_voltage_get (devp_t * param, void * value);

static devp_t m_dp_battery_voltage =
{
    .name = "battery_vol_mW",
    .type = DP_TYPE_UINT16,
    .size = sizeof(uint16_t),
    .persist = false,
    .getf = dp_battery_voltage_get,
    .setf = 0
};

int dp_battery_voltage_get (devp_t * param, void * value)
{
    *((uint16_t*)value) = 0U;
    uint16_t milliVolts;
	#ifdef DIRECT_BATTERY_SUPPLY
	{
		SupplyVoltageReader_init();
		milliVolts = SupplyVoltageReader_read();
		SupplyVoltageReader_deinit();
	}
	#else
	{
		read_battery_voltage(&milliVolts);
	}
	#endif
    *((uint16_t*)value) = milliVolts;
    return sizeof(uint16_t);
}

void devp_supply_voltage_init ()
{
    devp_register(&m_dp_battery_voltage);
}
