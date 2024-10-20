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

#ifndef TBCO2
static bool m_scd_service_mode = true;
#endif//TBCO2

static uint32_t m_scd_service_mode_start_s;

static bool get_service_mode ()
{
    #ifdef TBCO2
        return tbco2_get_scd_service_mode();
    #else
        return m_scd_service_mode;
    #endif
}

// -----------------------------------------------------------------------------
// Functions useful when not using with TbCO2 application ----------------------
#ifndef TBCO2
static bool scd_disable()
{
    return true;
}

static bool scd_enable()
{
    // uint16_t error = 0;

    // Clean up potential SCD40 states
    // scd4x_wake_up();
    // scd4x_stop_periodic_measurement();
    // scd4x_reinit();

    // error = scd4x_start_periodic_measurement();
    // if (error)
    // {
    //     warn1("scd4x_start_periodic_measurement: %i", error);
    //     return false;
    // }
    return true;
}
#endif//TBCO2

#ifndef TBCO2
static bool scd_get(int32_t * pco2, uint16_t * ptmp, int32_t * phum)
{
    uint16_t temp_samples_co2;
    int32_t temp_samples_tmp;
    int32_t temp_samples_hum;

    // uint16_t scd4x_data_ready;
    uint16_t error;
    bool data_ready;
    // error = scd4x_get_data_ready_status(&scd4x_data_ready);
    error = scd4x_get_data_ready_flag(&data_ready);
    if ((NO_ERROR == error) && (true == data_ready))
    {
        error = scd4x_read_measurement(&temp_samples_co2, &temp_samples_tmp, &temp_samples_hum);
        if (NO_ERROR == error)
        {
            *pco2 = temp_samples_co2;
            *ptmp = temp_samples_tmp / 100;
            *phum = temp_samples_hum / 100;
            debug1("CO2: %d ppm, T: %d/10 *C, HUM: %d/10 %%RH",
                (int)(temp_samples_co2),
                (int)(temp_samples_tmp/100),
                (int)(temp_samples_hum/100));
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
    *((bool*)value) = get_service_mode();
    return sizeof(bool);
}

static int dp_scd4x_service_mode_set(devp_t * param, bool init, const void * value, uint8_t size)
{
#ifdef TBCO2
    //Disabling this functionality when using on TBCO2
#else
    bool mode = *(bool*)value;
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
#endif //TBCO2
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
    if (get_service_mode())
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
    if (get_service_mode())
    {
        uint64_t serial_number = 0;

#ifdef TBCO2
        uint16_t error = tbco2_get_scd4x_serial(&serial_number);
        if(NO_ERROR != error)
        {
            return DEVP_EFAIL;
        }
#else
        uint16_t nums[4] = { 0 };
        if (NO_ERROR != scd4x_get_serial_number(&nums[1], &nums[2], &nums[3]))
        {
            return DEVP_EFAIL;
        }
        serial_number |= (uint64_t) nums[0] << 0;
        serial_number |= (uint64_t) nums[1] << 16;
        serial_number |= (uint64_t) nums[2] << 32;
        serial_number |= (uint64_t) nums[3] << 48;
#endif //TBCO2

        *((uint64_t*)value) = serial_number;
        return sizeof(uint64_t);
    }
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
    if (get_service_mode())
    {
        int32_t co2;
#ifdef TBCO2
        co2 = tbco2_get_scd_co2();
#else
        if( ! scd_get(&co2, NULL, NULL))
        {
            return DEVP_EFAIL;
        }
#endif //TBCO2
        *((uint16_t*)value) = co2;
        return sizeof(uint16_t);
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
    if (get_service_mode())
    {
        uint16_t temp;
#ifdef TBCO2
        temp = tbco2_get_scd_temperature();
#else
        if ( ! scd_get(NULL, &temp, NULL))
        {
            return DEVP_EFAIL;
        }
#endif //TBCO2
        *((int32_t*)value) = temp;
        return sizeof(int32_t);
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
    if (get_service_mode())
    {
        int32_t hum;
#ifdef TBCO2
        hum = tbco2_get_scd_humidity();
#else
        if( ! scd_get(NULL, NULL, &hum))
        {
            return DEVP_EFAIL;
        }
#endif //TBCO2
        *((int32_t*)value) = hum * 10;
        return sizeof(int32_t);
    }
    return DEVP_EOFF;
}
// -----------------------------------------------------------------------------

static int32_t m_scd4x_tempoffs_default;

static int dp_scd4x_tempoffs_default_get (devp_t * param, void * value);
static int dp_scd4x_tempoffs_default_set (devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_scd4x_tempoffs_default = {
	.name = "scd4x_tffs_def",
	.type = DP_TYPE_INT32,
	.size = sizeof(int32_t),
	.persist = true,
	.getf = dp_scd4x_tempoffs_default_get,
	.setf = dp_scd4x_tempoffs_default_set
};

static int dp_scd4x_tempoffs_default_get (devp_t * param, void * value)
{
	*((int32_t*)value) = m_scd4x_tempoffs_default;
	return sizeof(int32_t);
}

static int dp_scd4x_tempoffs_default_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	m_scd4x_tempoffs_default = *((int32_t*)value);
	return sizeof(int32_t);
}

void dp_scd4x_tempoffs_default_init (int32_t temp_offset)
{
    m_scd4x_tempoffs_default = temp_offset;
    devp_register(&m_dp_scd4x_tempoffs_default);
}

int32_t devp_scd4x_tempoffs_default_get ()
{
    return m_scd4x_tempoffs_default;
}


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
    if (get_service_mode())
    {
        int16_t error = 0;
#ifdef TBCO2
        error = tbco2_get_scd4x_temperature_offset(value);
#else
        error = scd4x_get_temperature_offset(value);
#endif //TBCO2
        if(NO_ERROR == error)
        {
            return sizeof(int32_t);
        }
        warn1("tbco2_scd4x_get temp_offset_error: %d", error);
        return DEVP_EINVAL;
    }
    return DEVP_EOFF;
}

static int dp_scd4x_tempoffs_set(devp_t * param, bool init, const void * value, uint8_t size)
{
    if (sizeof(int32_t) == size)
    {
        if (get_service_mode())
        {
            int32_t temperature_offset = *((int32_t*)value);
#ifdef TBCO2
            if(0 == tbco2_set_scd4x_temperature_offset(temperature_offset))
#else
            if(0 == scd4x_set_temperature_offset(temperature_offset))
#endif
            {
                return sizeof(int32_t);
            }

            return DEVP_EINVAL;
        }
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
        if (get_service_mode())
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
    if (get_service_mode())
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
        if (get_service_mode())
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
