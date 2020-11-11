/**
 * LTC4015 through DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */

#include "devp_ltc.h"

#include "stddef.h"

#include "ltc.h"

// -----------------------------------------------------------------------------
static int dp_ltc_battery_voltage_get (devp_t * param, void * value);

static devp_t m_dp_ltc_battery_voltage =
{
    .name = "battery_voltage(mV)",
    .type = DP_TYPE_UINT16,
    .size = sizeof(uint16_t),
    .persist = false,
    .getf = dp_ltc_battery_voltage_get,
    .setf = NULL
};

int dp_ltc_battery_voltage_get (devp_t * param, void * value)
{
    const bool is_ltc_initialized = ltc4015_init();

    if (true == is_ltc_initialized)
    {
        *((uint16_t*)value) = ltc4015_battery_read();
    }
    else
    {
        *((uint16_t*)value) = 0U;
    }

    return sizeof(uint16_t);
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_ltc_solar_voltage_get (devp_t * param, void * value);

static devp_t m_dp_ltc_solar_voltage =
{
    .name = "solar_voltage(mV)",
    .type = DP_TYPE_UINT16,
    .size = sizeof(uint16_t),
    .persist = false,
    .getf = dp_ltc_solar_voltage_get,
    .setf = NULL
};

int dp_ltc_solar_voltage_get (devp_t * param, void * value)
{
    const bool is_ltc_initialized = ltc4015_init();

    if (true == is_ltc_initialized)
    {
        *((uint16_t*)value) = ltc4015_panelV_read();
    }
    else
    {
        *((uint16_t*)value) = 0U;
    }

    return sizeof(uint16_t);
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_ltc_solar_current_get (devp_t * param, void * value);

static devp_t m_dp_ltc_solar_current =
{
    .name = "solar_current(mA)",
    .type = DP_TYPE_INT16,
    .size = sizeof(int16_t),
    .persist = false,
    .getf = dp_ltc_solar_current_get,
    .setf = NULL
};

int dp_ltc_solar_current_get (devp_t * param, void * value)
{
    const bool is_ltc_initialized = ltc4015_init();

    if (true == is_ltc_initialized)
    {
        *((int16_t*)value) = ltc4015_charge_current_read();
    }
    else
    {
        *((int16_t*)value) = 0;
    }

    return sizeof(int16_t);
}
// -----------------------------------------------------------------------------

void devp_ltc_init ()
{
    devp_register(&m_dp_ltc_battery_voltage);
    devp_register(&m_dp_ltc_solar_voltage);
    devp_register(&m_dp_ltc_solar_current);
}