/**
 * SCD4X service mode parameters
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */

#include "devp_scd4x_service_mode.h"

#include <string.h>
#include "sensors.h"
#include "retargeti2c.h"
#include "retargeti2cconfig.h"
#include "platform_i2c.h"
#include "cmsis_os2_ext.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"
#include "scd4x_i2c.h"
#ifdef TBCO2
#include "tbco2_app.h"
#endif // TBCO2

#include "loglevels.h"
#define __MODUUL__ "scd4xs"
#define __LOG_LEVEL__ (LOG_LEVEL_devp_scd4x_service_mode & BASE_LOG_LEVEL)
#include "log.h"

#ifdef SCD4X_ADDRESS
#define SCD4X_I2C_ADDRESS SCD4X_ADDRESS
#else
#define SCD4X_I2C_ADDRESS 0x62
#endif

#define SCD4X_CMD_SET_FORCED_RECALIBRATION 0x362F

static bool m_scd_service_mode = false;
static uint32_t m_scd_service_mode_start_s;

// -----------------------------------------------------------------------------
// Functions useful when not using with TbCO2 application ----------------------
static bool scd_disable()
{

    ext_sensor_power_off();

    RETARGET_I2CDeinit();

    platform_i2c_release(RETARGET_I2C_DEV);

    return true;
}

static bool scd_enable()
{
    if(platform_i2c_request(RETARGET_I2C_DEV, 10000))
    {
        uint16_t error = 0;
        RETARGET_I2CInit();

        ext_sensor_power_on();

        osDelay(3000);

        // Clean up potential SCD40 states
        scd4x_wake_up();
        scd4x_stop_periodic_measurement();
        scd4x_reinit();

        error = scd4x_start_periodic_measurement();
        if (error)
        {
            warn1("scd4x_start_periodic_measurement: %i", error);
        }
    }
    else
    {
        err1("I2C unavailable!");
    }
    return false;
}

#ifndef TBCO2
static bool scd_get(int32_t * pco2, uint16_t * ptmp, int32_t * phum)
{
    uint16_t temp_samples_co2;
    int32_t temp_samples_tmp;
    int32_t temp_samples_hum;

    uint16_t scd4x_data_ready;
    uint16_t error;
    error = scd4x_get_data_ready_status(&scd4x_data_ready);
    if (NO_ERROR == error)
    {
        error = scd4x_read_measurement(&temp_samples_co2, &temp_samples_tmp, &temp_samples_hum);
        if (NO_ERROR == error)
        {
            *pco2 = temp_samples_co2;
            *ptmp = temp_samples_tmp / 1000;
            *phum = temp_samples_hum / 1000;
            debug1("CO2: %d ppm, T: %d/10 *C, HUM: %d/10 %%RH",
                (int)(temp_samples_co2),
                (int)((temp_samples_tmp/1000) * 10),
                (int)((temp_samples_hum/1000) * 10));
            return error;
        }
        else
        {
            err1("read scd4x");
            return error;
        }
    }
    else
    {
        return error;
    }
}

static int16_t scd4x_get_forced_calibration(uint16_t * co2_ppm)
{
    uint16_t word;
    int16_t ret = sensirion_i2c_read_cmd(SCD4X_I2C_ADDRESS,
                                 SCD4X_CMD_SET_FORCED_RECALIBRATION, &word,
                                 SENSIRION_NUM_WORDS(word));
    if (ret != NO_ERROR)
    {
        return ret;
    }

    *co2_ppm = word;

    return NO_ERROR;
}

#endif //TBCO2
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd4x_service_mode_get(devp_t * param, void * value);
static int dp_scd4x_service_mode_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_scd4x_service_mode = {
    .name = "scd4x_service_mode",
    .type = DP_TYPE_BOOL,
    .size = sizeof(bool),
    .persist = false,
    .getf = dp_scd4x_service_mode_get,
    .setf = dp_scd4x_service_mode_set
};

static int dp_scd4x_service_mode_get(devp_t * param, void * value)
{
#ifdef TBCO2
    *((bool*)value) = tbco2_get_scd_service_mode();
    return sizeof(bool);
#else
    *((bool*)value) = m_scd_service_mode;
    return sizeof(bool);
#endif
}
static int dp_scd4x_service_mode_set(devp_t * param, bool init, const void * value, uint8_t size)
{
    bool mode;
#ifdef TBCO2
    //Disabling this functionality when using on TBCO2
    return 0;
#else
    mode = *(bool*)value;
#endif //TBCO2

    if (mode != m_scd_service_mode)
    {
        if (mode)
        {
            if (scd_enable())
            {
                m_scd_service_mode = true;
                m_scd_service_mode_start_s = osCounterGetSecond();
            }
        }
        else
        {
            if (scd_disable())
            {
                m_scd_service_mode = false;
            }
        }
    }
    return 0;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd4x_service_time_get(devp_t * param, void * value);

static devp_t m_dp_scd4x_service_time = {
    .name = "scd4x_service_time",
    .type = DP_TYPE_UINT32,
    .size = sizeof(uint32_t),
    .persist = false,
    .getf = dp_scd4x_service_time_get,
    .setf = NULL
};

static int dp_scd4x_service_time_get(devp_t * param, void * value)
{
    if (m_scd_service_mode)
    {
        *((uint32_t*)value) = osCounterGetSecond() - m_scd_service_mode_start_s;
    }
    else
    {
        *((uint32_t*)value) = 0;
    }
    return sizeof(uint32_t);
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd4x_serial_get(devp_t * param, void * value);

static devp_t m_dp_scd4x_serial = {
    .name = "scd4x_serial",
    .type = DP_TYPE_UINT64,
    .size = sizeof(uint64_t),
    .persist = false,
    .getf = dp_scd4x_serial_get,
    .setf = NULL
};

static int dp_scd4x_serial_get(devp_t * param, void * value)
{
#ifdef TBCO2
    m_scd_service_mode = tbco2_get_scd_service_mode();
    if(m_scd_service_mode)
    {
        uint64_t temp = 0;
        uint16_t error = tbco2_get_scd4x_serial(&temp);
        if(NO_ERROR == error)
        {
            *((uint64_t*)value) = temp;
            return sizeof(uint64_t);
        }
        else
        {
            return error;
        }
    }
#else
    if(m_scd_service_mode)
    {
        uint64_t serial_number = 0;
        uint16_t nums[4];
        for (int i = 0; i < 4 ; i++ )
        {
            nums[i] = 0;
        }

        if (0 == scd4x_get_serial_number(&nums[1], &nums[2], &nums[3]))
        {

            serial_number |= (uint64_t) nums[0] << 0;
            serial_number |= (uint64_t) nums[1] << 16;
            serial_number |= (uint64_t) nums[2] << 32;
            serial_number |= (uint64_t) nums[3] << 48;
            *((uint64_t*)value) = serial_number;
        }

        return 0;
    }
#endif //TBCO2
    return DEVP_EOFF;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd4x_co2_get(devp_t * param, void * value);

static devp_t m_dp_scd4x_co2 = {
    .name = "scd4x_co2",
    .type = DP_TYPE_UINT16,
    .size = sizeof(uint16_t),
    .persist = false,
    .getf = dp_scd4x_co2_get,
    .setf = NULL
};

static int dp_scd4x_co2_get(devp_t * param, void * value)
{
    if (m_scd_service_mode)
    {
        int32_t co2;
#ifdef TBCO2
        co2 = tbco2_get_scd_co2();
        *((uint16_t*)value) = co2;
        return sizeof(uint16_t);
#else
        if(scd_get(&co2, NULL, NULL))
        {
            *((uint16_t*)value) = co2;
            return sizeof(uint16_t);
        }
#endif //TBCO2
        return 0;
    }
    return DEVP_EOFF;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd4x_temp_get(devp_t * param, void * value);

static devp_t m_dp_scd4x_temp = {
    .name = "scd4x_temp",
    .type = DP_TYPE_INT32,
    .size = sizeof(int32_t),
    .persist = false,
    .getf = dp_scd4x_temp_get,
    .setf = NULL
};

static int dp_scd4x_temp_get(devp_t * param, void * value)
{
    if (m_scd_service_mode)
    {
        uint16_t temp;
#ifdef TBCO2
        temp = tbco2_get_scd_temperature();
        *((int32_t*)value) = temp * 10;
        return sizeof(int32_t);
#else
        if(scd_get(NULL, &temp, NULL))
        {
            *((int32_t*)value) = temp * 10;
            return sizeof(int32_t);
        }
#endif //TBCO2
        return 0;
    }
    return DEVP_EOFF;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd4x_hum_get(devp_t * param, void * value);

static devp_t m_dp_scd4x_hum = {
    .name = "scd4x_hum",
    .type = DP_TYPE_INT32,
    .size = sizeof(int32_t),
    .persist = false,
    .getf = dp_scd4x_hum_get,
    .setf = NULL
};

static int dp_scd4x_hum_get(devp_t * param, void * value)
{
    if (m_scd_service_mode)
    {
        int32_t hum;
#ifdef TBCO2
        hum = tbco2_get_scd_humidity();
        *((int32_t*)value) = hum * 10;
        return sizeof(int32_t);
#else

        if(scd_get(NULL, NULL, &hum))
        {
            *((int32_t*)value) = hum * 10;
            return sizeof(int32_t);
        }
        return 0;
#endif //TBCO2
    }
    return DEVP_EOFF;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd4x_tempoffs_get(devp_t * param, void * value);
static int dp_scd4x_tempoffs_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_scd4x_tempoffs = {
    .name = "scd4x_tempoffs",
    .type = DP_TYPE_INT32,
    .size = sizeof(int32_t),
    .persist = false,
    .getf = dp_scd4x_tempoffs_get,
    .setf = dp_scd4x_tempoffs_set
};

static int dp_scd4x_tempoffs_get(devp_t * param, void * value)
{
    if (m_scd_service_mode)
    {
#ifdef TBCO2
        int16_t error = 0;
        error = tbco2_get_scd4x_temperature_offset(value);
        if(NO_ERROR == error)
        {
            return sizeof(int32_t);
        }
        else
        {
            warn1("tbco2_scd4x_get temp_offset_error: %d", error);
        }
#else
        if(0 == scd4x_get_temperature_offset(value))
        {
            return sizeof(int32_t);
        }
#endif //TBCO2
        return DEVP_EINVAL;
    }
    return DEVP_EOFF;
}

static int dp_scd4x_tempoffs_set(devp_t * param, bool init, const void * value, uint8_t size)
{
    if (sizeof(int32_t) == size)
    {
#ifdef TBCO2
        if(m_scd_service_mode)
        {
            int32_t temperature_offset = *((int32_t*)value);
            if(0 == tbco2_set_scd4x_temperature_offset(temperature_offset))
            {
                return 0;
            }
            return DEVP_EINVAL;
        }
#else
        if (m_scd_service_mode)
        {
            int32_t temperature_offset = *((int32_t*)value);
            if(0 == scd4x_set_temperature_offset(temperature_offset))
            {
                return 0;
            }
            return DEVP_EINVAL;
        }
#endif
        return DEVP_EOFF;
    }
    return DEVP_ESIZE;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd4x_frc_get(devp_t * param, void * value);
static int dp_scd4x_frc_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_scd4x_frc = {
    .name = "scd4x_co2_frc",
    .type = DP_TYPE_INT16,
    .size = sizeof(int16_t),
    .persist = false,
    .getf = dp_scd4x_frc_get,
    .setf = dp_scd4x_frc_set
};

static int16_t m_scd4x_frc = 0;

static int dp_scd4x_frc_get(devp_t * param, void * value)
{
    memcpy(value, &m_scd4x_frc, sizeof(m_scd4x_frc));
    return sizeof(m_scd4x_frc);
}

static int dp_scd4x_frc_set(devp_t * param, bool init, const void * value, uint8_t size)
{
    if (sizeof(int16_t) == size)
    {
        if (m_scd_service_mode)
        {
            uint16_t frc = *((uint16_t*)value);
            uint16_t result = NO_ERROR;
            uint16_t error = NO_ERROR;

#ifdef TBCO2
            error = tbco2_set_scd4x_forced_calibration(frc, &result);
#else
            error = scd4x_perform_forced_recalibration(frc, &result);
#endif //TBCO2
            //debug1("acquire frc");
            if(NO_ERROR == error)
            {
                if (0xFFFF == result)
                {
                    m_scd4x_frc = -1;
                    warn1("scd4x_set_frc_result 0xFFFF");
                    return DEVP_EFAIL;
                }
                m_scd4x_frc = frc;
                debug1("offset was %"PRIu16, result);
                return sizeof(int16_t);
            }
            warn1("scd4x_set_frc error error: %d frc: %d result: %d", error, frc, result);
            m_scd4x_frc = -2;
            return DEVP_EFAIL;
        }
        return DEVP_EOFF;
    }
    return DEVP_ESIZE;

}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd4x_asc_get(devp_t * param, void * value);
static int dp_scd4x_asc_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_scd4x_asc = {
    .name = "scd4x_co2_asc",
    .type = DP_TYPE_UINT16,
    .size = sizeof(uint16_t),
    .persist = false,
    .getf = dp_scd4x_asc_get,
    .setf = dp_scd4x_asc_set
};

static int dp_scd4x_asc_get(devp_t * param, void * value)
{
    if (m_scd_service_mode)
    {
        uint16_t asc;
#ifdef TBCO2
        if(NO_ERROR == tbco2_get_scd4x_automatic_self_calibration(&asc))
#else
        if(NO_ERROR == scd4x_get_automatic_self_calibration(&asc))
#endif //TBCO2
        {
            *((uint16_t*)value) = asc;
        }

        return sizeof(uint16_t);
    }
    return DEVP_EOFF;
}

static int dp_scd4x_asc_set(devp_t * param, bool init, const void * value, uint8_t size)
{
    if (sizeof(uint16_t) == size)
    {
        if (m_scd_service_mode)
        {
            uint16_t asc = *((uint16_t*)value);
#ifdef TBCO2
            if(NO_ERROR == tbco2_set_scd4x_automatic_self_calibration(asc))
#else
            if(NO_ERROR == scd4x_set_automatic_self_calibration(asc))
#endif //TBCO2
            {
                return NO_ERROR;
            }
            return DEVP_EINVAL;
        }
        return DEVP_EOFF;
    }
    return DEVP_ESIZE;
}
// -----------------------------------------------------------------------------

void devp_scd4x_service_mode_init()
{
    devp_register(&m_dp_scd4x_service_mode);
    devp_register(&m_dp_scd4x_service_time);
    devp_register(&m_dp_scd4x_serial);
    devp_register(&m_dp_scd4x_temp);
    devp_register(&m_dp_scd4x_hum);
    devp_register(&m_dp_scd4x_co2);
    devp_register(&m_dp_scd4x_tempoffs);
    devp_register(&m_dp_scd4x_frc);
    devp_register(&m_dp_scd4x_asc);
}
