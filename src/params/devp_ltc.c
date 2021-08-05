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

#include "ltc4015_charger.h"

// -----------------------------------------------------------------------------
static int dp_ltc_battery_voltage_get (devp_t * param, void * value);

static devp_t m_dp_ltc_battery_voltage =
{
    .name = "battery_vol_mV",
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
        ltc4015_data_t ltc4015_data;
        ltc4015_charger_read_data(&ltc4015_data);

        if (0.0 < ltc4015_data.battery_voltage_V)
        {
            *((uint16_t*)value) = (uint16_t)(ltc4015_data.battery_voltage_V * 1000.0);
        }

        platform_i2c_release(RETARGET_I2C_DEV);

        return sizeof(uint16_t);
    }
    return DEVP_EBUSY;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_ltc_solar_voltage_get (devp_t * param, void * value);

static devp_t m_dp_ltc_solar_voltage =
{
    .name = "solar_vol_mV",
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
        ltc4015_data_t ltc4015_data;
        ltc4015_charger_read_data(&ltc4015_data);

        if (0.0 < ltc4015_data.input_voltage_V)
        {
            *((uint16_t*)value) = (uint16_t)(ltc4015_data.input_voltage_V * 1000.0);
        }

        platform_i2c_release(RETARGET_I2C_DEV);

        return sizeof(uint16_t);
    }
    return DEVP_EBUSY;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_ltc_solar_current_get (devp_t * param, void * value);

static devp_t m_dp_ltc_solar_current =
{
    .name = "solar_curr_mA",
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
        ltc4015_data_t ltc4015_data;
        ltc4015_charger_read_data(&ltc4015_data);

        if (0.0 != ltc4015_data.battery_current_mA)
        {
            *((int16_t*)value) = (int16_t)(ltc4015_data.battery_current_mA);
        }

        platform_i2c_release(RETARGET_I2C_DEV);

        return sizeof(int16_t);
    }
    return DEVP_EBUSY;
}
// -----------------------------------------------------------------------------

void devp_ltc_init ()
{
    devp_register(&m_dp_ltc_battery_voltage);
    devp_register(&m_dp_ltc_solar_voltage);
    devp_register(&m_dp_ltc_solar_current);
}
