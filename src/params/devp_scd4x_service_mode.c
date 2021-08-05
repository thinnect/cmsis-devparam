/**
 * SCD4X service mode parameters
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */

#include "devp_scd4x_service_mode.h"

#include <string.h>
#include "sensors.h"
#include "cmsis_os2_ext.h"
#include "sensirion_common.h"
#include "scd4x_i2c.h"
#ifdef TBCO2
#include "tbco2_app.h"
#endif // TBCO2

#include "loglevels.h"
#define __MODUUL__ "scd4xs"
#define __LOG_LEVEL__ (LOG_LEVEL_devp_scd4x_service_mode & BASE_LOG_LEVEL)
#include "log.h"

static bool m_scd_service_mode = false;
static uint32_t m_scd_service_mode_start_s;

// -----------------------------------------------------------------------------
// Functions useful when not using with TbCO2 application ----------------------
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
		uint16_t error = 0;
        RETARGET_I2CInit();

		// Enable power to SCD30 and wait for it to start
		ext_sensor_power_on();

        osDelay(3000);
	    sensirion_i2c_hal_init();

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

	if (NO_ERROR == scd4x_get_data_ready_status(&scd4x_data_ready))
	{
		if (NO_ERROR == scd4x_read_measurement(&temp_samples_co2, &temp_samples_tmp, &temp_samples_hum))
		{
			*pco2 = temp_samples_co2;
			*ptmp = temp_samples_tmp / 1000;
			*phum = temp_samples_hum / 1000;
			debug1("CO2: %d ppm, T: %d/10 *C, HUM: %d/10 %%RH",
				(int)(temp_samples_co2),
				(int)((temp_samples_tmp/1000) * 10),
				(int)((temp_samples_hum/1000) * 10));
		}
		else
		{
			err1("read");
		}
        osdelay(100);
	}

}
#endif //TBCO2
// -----------------------------------------------------------------------------

// -----------------------------------------------------------------------------
static int dp_scd4x_service_mode_get(devp_t * param, void * value);
static int dp_scd4x_service_mode_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_scd4x_service_mode = {
	.name = "scd30_service_mode",
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
        *((uint64_t*)value) = temp;
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
		float co2;
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
		float temp;
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
		float hum;
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

void devp_scd30_service_mode_init()
{
	devp_register(&m_dp_scd4x_service_mode);
	devp_register(&m_dp_scd4x_service_time);
	devp_register(&m_dp_scd4x_serial);
	devp_register(&m_dp_scd4x_temp);
	devp_register(&m_dp_scd4x_hum);
	devp_register(&m_dp_scd4x_co2);
	devp_register(&m_dp_scd_tempoffs);
	devp_register(&m_dp_scd_frc);
	devp_register(&m_dp_scd_asc);
}
