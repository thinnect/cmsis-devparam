/**
 * SCD30 service mode parameters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#include "devp_scd30_service_mode.h"

#include <string.h>

#include "sensors.h"

#include "cmsis_os2_ext.h"

#include "scd30.h"
#include "retargeti2c.h"
#include "retargeti2cconfig.h"
#include "platform_i2c.h"

#include "loglevels.h"
#define __MODUUL__ "scd30s"
#define __LOG_LEVEL__ (LOG_LEVEL_devp_scd30_service_mode & BASE_LOG_LEVEL)
#include "log.h"

static bool m_scd_service_mode = false;
static uint32_t m_scd_service_mode_start_s;


// -----------------------------------------------------------------------------
// Implement SCD GET commands which are not in the driver, but in datasheet
// -----------------------------------------------------------------------------
#define SCD30_CMD_SET_TEMPERATURE_OFFSET 0x5403
#define SCD30_CMD_SET_FORCED_RECALIBRATION 0x5204
#define SCD30_CMD_GET_FIRMWARE_VERSION 0xD100
#ifdef SCD30_ADDRESS
static const uint8_t SCD30_I2C_ADDRESS = SCD30_ADDRESS;
#else
static const uint8_t SCD30_I2C_ADDRESS = 0x61;
#endif

static int16_t scd30_get_forced_calibration(uint16_t * co2_ppm)
{
    uint16_t word;
    int16_t ret = sensirion_i2c_read_cmd(SCD30_I2C_ADDRESS,
                                 SCD30_CMD_SET_FORCED_RECALIBRATION, &word,
                                 SENSIRION_NUM_WORDS(word));
    if (ret != STATUS_OK)
    {
        return ret;
    }

    *co2_ppm = word;

    return STATUS_OK;
}

static int16_t scd30_get_temperature_offset(uint16_t * temperature_offset)
{
    uint16_t word;
    int16_t ret = sensirion_i2c_read_cmd(SCD30_I2C_ADDRESS,
                                 SCD30_CMD_SET_TEMPERATURE_OFFSET, &word,
                                 SENSIRION_NUM_WORDS(word));
    if (ret != STATUS_OK)
    {
        return ret;
    }

    *temperature_offset = word;

    return STATUS_OK;
}

static int16_t scd30_get_firmware_version(uint16_t * firmware_version)
{
    uint16_t word;
    int16_t ret = sensirion_i2c_read_cmd(SCD30_I2C_ADDRESS,
                                 SCD30_CMD_GET_FIRMWARE_VERSION, &word,
                                 SENSIRION_NUM_WORDS(word));
    if (ret != STATUS_OK)
    {
        return ret;
    }

    *firmware_version = word;

    return STATUS_OK;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static bool scd_disable()
{
	// Disable power to SCD30
	ext_sensor_power_off();

	RETARGET_I2CDeinit();

	platform_i2c_release(RETARGET_I2C_DEV);

	return true;
}

static bool scd_enable()
{
	if(platform_i2c_request(RETARGET_I2C_DEV, 10000))
	{
		RETARGET_I2CInit();

		// Enable power to SCD30 and wait for it to start
		ext_sensor_power_on();

		osDelay(2000);
		for(uint8_t i=0;i<3;i++)
		{
			osDelay(3000);
			if (STATUS_OK == scd30_probe())
			{
				scd30_set_measurement_interval(2);
				osDelay(20);
				scd30_start_periodic_measurement(0);
				return true;
			}
		}

		scd_disable();
	}
	else
	{
		err1("I2C unavailable!");
	}
	return false;
}

static bool scd_get(float * pco2, float * ptmp, float * phum)
{
	uint16_t scd30_data_ready = 0;
	for(uint8_t i=0; i<100; i++)
	{
		if ((STATUS_OK == scd30_get_data_ready(&scd30_data_ready)) && scd30_data_ready)
		{
			float co2;
			float tmp;
			float hum;
			if (STATUS_OK == scd30_read_measurement(&co2, &tmp, &hum))
			{
				debug1("CO2: %d ppm, T: %d/10 *C, HUM: %d/10 %%RH", (int)(co2), (int)(tmp * 10), (int)(hum * 10));
				if (NULL != pco2)
				{
					*pco2 = co2;
				}
				if (NULL != ptmp)
				{
					*ptmp = tmp;
				}
				if (NULL != phum)
				{
					*phum = hum;
				}
				return true;
			}
		}
		osDelay(100);
	}
	return false;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd_service_mode_get(devp_t * param, void * value);
static int dp_scd_service_mode_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_scd_service_mode = {
	.name = "scd_service_mode",
	.type = DP_TYPE_BOOL,
	.size = sizeof(bool),
	.persist = false,
	.getf = dp_scd_service_mode_get,
	.setf = dp_scd_service_mode_set
};

static int dp_scd_service_mode_get(devp_t * param, void * value)
{
	*((bool*)value) = m_scd_service_mode;
	return sizeof(bool);
}

static int dp_scd_service_mode_set(devp_t * param, bool init, const void * value, uint8_t size)
{
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
	return 0;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd_service_time_get(devp_t * param, void * value);

static devp_t m_dp_scd_service_time = {
	.name = "scd_service_time",
	.type = DP_TYPE_UINT32,
	.size = sizeof(uint32_t),
	.persist = false,
	.getf = dp_scd_service_time_get,
	.setf = NULL
};

static int dp_scd_service_time_get(devp_t * param, void * value)
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
static int dp_scd_serial_get(devp_t * param, void * value);

static devp_t m_dp_scd_serial = {
	.name = "scd_serial",
	.type = DP_TYPE_STRING,
	.size = 40,
	.persist = false,
	.getf = dp_scd_serial_get,
	.setf = NULL
};

static int dp_scd_serial_get(devp_t * param, void * value)
{
	if(m_scd_service_mode)
	{
		if (0 == scd30_read_serial(value))
		{
			return strlen(value);
		}
		((char*)value)[0] = '?';
		return 1;
	}
	return DEVP_EOFF;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd_firmware_get(devp_t * param, void * value);

static devp_t m_dp_scd_firmware = {
	.name = "scd_firmware",
	.type = DP_TYPE_STRING,
	.size = 32,
	.persist = false,
	.getf = dp_scd_firmware_get,
	.setf = NULL
};

static int dp_scd_firmware_get(devp_t * param, void * value)
{
	if(m_scd_service_mode)
	{
		uint16_t firmware;
		if (0 == scd30_get_firmware_version(&firmware))
		{
			snprintf(value, param->size, "%d.%d", (int)(firmware >> 8), (int)(firmware & 0xFF));
			return strlen(value);
		}
		return DEVP_EINVAL;
	}
	return DEVP_EOFF;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd_co2_get(devp_t * param, void * value);

static devp_t m_dp_scd_co2 = {
	.name = "scd_co2",
	.type = DP_TYPE_UINT16,
	.size = sizeof(uint16_t),
	.persist = false,
	.getf = dp_scd_co2_get,
	.setf = NULL
};

static int dp_scd_co2_get(devp_t * param, void * value)
{
	if (m_scd_service_mode)
	{
		float co2;
		if(scd_get(&co2, NULL, NULL))
		{
			*((uint16_t*)value) = co2;
			return sizeof(uint16_t);
		}
		return 0;
	}
	return DEVP_EOFF;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd_temp_get(devp_t * param, void * value);

static devp_t m_dp_scd_temp = {
	.name = "scd_temp",
	.type = DP_TYPE_INT16,
	.size = sizeof(int16_t),
	.persist = false,
	.getf = dp_scd_temp_get,
	.setf = NULL
};

static int dp_scd_temp_get(devp_t * param, void * value)
{
	if (m_scd_service_mode)
	{
		float temp;
		if(scd_get(NULL, &temp, NULL))
		{
			*((int16_t*)value) = temp * 10;
			return sizeof(int16_t);
		}
		return 0;
	}
	return DEVP_EOFF;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd_hum_get(devp_t * param, void * value);

static devp_t m_dp_scd_hum = {
	.name = "scd_hum",
	.type = DP_TYPE_UINT16,
	.size = sizeof(uint16_t),
	.persist = false,
	.getf = dp_scd_hum_get,
	.setf = NULL
};

static int dp_scd_hum_get(devp_t * param, void * value)
{
	if (m_scd_service_mode)
	{
		float hum;
		if(scd_get(NULL, NULL, &hum))
		{
			*((uint16_t*)value) = hum * 10;
			return sizeof(uint16_t);
		}
		return 0;
	}
	return DEVP_EOFF;
}
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd_tempoffs_get(devp_t * param, void * value);
static int dp_scd_tempoffs_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_scd_tempoffs = {
	.name = "scd_tempoffs",
	.type = DP_TYPE_UINT16,
	.size = sizeof(uint16_t),
	.persist = false,
	.getf = dp_scd_tempoffs_get,
	.setf = dp_scd_tempoffs_set
};

static int dp_scd_tempoffs_get(devp_t * param, void * value)
{
	if (m_scd_service_mode)
	{
		if(0 == scd30_get_temperature_offset(value))
		{
			return sizeof(uint16_t);
		}
		return DEVP_EINVAL;
	}
	return DEVP_EOFF;
}

static int dp_scd_tempoffs_set(devp_t * param, bool init, const void * value, uint8_t size)
{
	if (sizeof(uint16_t) == size)
	{
		if (m_scd_service_mode)
		{
			uint16_t temperature_offset = *((uint16_t*)value);
			if(0 == scd30_set_temperature_offset(temperature_offset))
			{
				return 0;
			}
			return DEVP_EINVAL;
		}
		return DEVP_EOFF;
	}
	return DEVP_ESIZE;
}
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
static int dp_scd_frc_get(devp_t * param, void * value);
static int dp_scd_frc_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_scd_frc = {
	.name = "scd_co2_frc",
	.type = DP_TYPE_UINT16,
	.size = sizeof(uint16_t),
	.persist = false,
	.getf = dp_scd_frc_get,
	.setf = dp_scd_frc_set
};

static int dp_scd_frc_get(devp_t * param, void * value)
{
	if (m_scd_service_mode)
	{
		if(0 == scd30_get_forced_calibration(value))
		{
			return sizeof(uint16_t);
		}
		return DEVP_EINVAL;
	}
	return DEVP_EOFF;
}

static int dp_scd_frc_set(devp_t * param, bool init, const void * value, uint8_t size)
{
	if (sizeof(uint16_t) == size)
	{
		if (m_scd_service_mode)
		{
			uint16_t frc = *((uint16_t*)value);
			if(0 == scd30_set_forced_recalibration(frc))
			{
				return 0;
			}
			return DEVP_EINVAL;
		}
		return DEVP_EOFF;
	}
	return DEVP_ESIZE;
}
// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------
static int dp_scd_asc_get(devp_t * param, void * value);
static int dp_scd_asc_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_scd_asc = {
	.name = "scd_co2_asc",
	.type = DP_TYPE_UINT8,
	.size = sizeof(uint8_t),
	.persist = false,
	.getf = dp_scd_asc_get,
	.setf = dp_scd_asc_set
};

static int dp_scd_asc_get(devp_t * param, void * value)
{
	if (m_scd_service_mode)
	{
		uint8_t asc;
		if(0 == scd30_get_automatic_self_calibration(&asc))
		{
			*((uint8_t*)value) = asc;
		}
		return sizeof(uint8_t);
	}
	return DEVP_EOFF;
}

static int dp_scd_asc_set(devp_t * param, bool init, const void * value, uint8_t size)
{
	if (sizeof(uint8_t) == size)
	{
		if (m_scd_service_mode)
		{
			uint8_t asc = *((uint8_t*)value);
			if(0 == scd30_enable_automatic_self_calibration(asc))
			{
				return 0;
			}
			return DEVP_EINVAL;
		}
		return DEVP_EOFF;
	}
	return DEVP_ESIZE;
}
// -----------------------------------------------------------------------------


void devp_scd30_service_mode_init()
{
	devp_register(&m_dp_scd_service_mode);
	devp_register(&m_dp_scd_service_time);
	devp_register(&m_dp_scd_serial);
	devp_register(&m_dp_scd_firmware);
	devp_register(&m_dp_scd_temp);
	devp_register(&m_dp_scd_hum);
	devp_register(&m_dp_scd_co2);
	devp_register(&m_dp_scd_tempoffs);
	devp_register(&m_dp_scd_frc);
	devp_register(&m_dp_scd_asc);
}
