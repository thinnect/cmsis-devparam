/**
 * LTC4015 through DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */

#include "devp_ltc.h"

#include "stddef.h"

#include "platform_io.h"
#include "retargeti2cconfig.h"
#include "platform_i2c.h"

#include "ltc.h"

// -----------------------------------------------------------------------------
static int dp_ltc_battery_voltage_get (devp_t * param, void * value);

static devp_t m_dp_ltc_battery_voltage =
{
    .name = "battery_voltage_(mV)",
    .type = DP_TYPE_UINT16,
    .size = sizeof(uint16_t),
    .persist = false,
    .getf = dp_ltc_battery_voltage_get,
    .setf = NULL
};

int dp_ltc_battery_voltage_get (devp_t * param, void * value)
{
    *((uint16_t*)value) = 0U;

    if(true == platform_i2c_request(RETARGET_I2C_DEV, 1000U))
    {
        const bool is_ltc_initialized = ltc4015_init();

        if (true == is_ltc_initialized)
        {
            *((uint16_t*)value) = ltc4015_battery_read();
        }
        platform_i2c_release(RETARGET_I2C_DEV);
    }
    return sizeof(uint16_t);
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_ltc_solar_voltage_get (devp_t * param, void * value);

static devp_t m_dp_ltc_solar_voltage =
{
    .name = "solar_voltage_(mV)",
    .type = DP_TYPE_UINT16,
    .size = sizeof(uint16_t),
    .persist = false,
    .getf = dp_ltc_solar_voltage_get,
    .setf = NULL
};

int dp_ltc_solar_voltage_get (devp_t * param, void * value)
{
    *((uint16_t*)value) = 0U;

    if(true == platform_i2c_request(RETARGET_I2C_DEV, 1000U))
    {
        const bool is_ltc_initialized = ltc4015_init();

        if (true == is_ltc_initialized)
        {
            *((uint16_t*)value) = ltc4015_panelV_read();
        }
        platform_i2c_release(RETARGET_I2C_DEV);
    }
    return sizeof(uint16_t);
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_ltc_solar_current_get (devp_t * param, void * value);

static devp_t m_dp_ltc_solar_current =
{
    .name = "solar_current_(mA)",
    .type = DP_TYPE_INT16,
    .size = sizeof(int16_t),
    .persist = false,
    .getf = dp_ltc_solar_current_get,
    .setf = NULL
};

int dp_ltc_solar_current_get (devp_t * param, void * value)
{
    *((int16_t*)value) = 0;

    if(true == platform_i2c_request(RETARGET_I2C_DEV, 1000U))
    {
        const bool is_ltc_initialized = ltc4015_init();

        if (true == is_ltc_initialized)
        {
            *((int16_t*)value) = ltc4015_charge_current_read();
        }
        platform_i2c_release(RETARGET_I2C_DEV);
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