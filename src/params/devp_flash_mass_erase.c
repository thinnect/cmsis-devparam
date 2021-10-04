/**
 * Mass-erase the dataflash.
 *
 * !!! WARNING !!!
 * ! Calling mass-erase will acquire the flash from a high-priority thread,
 * ! issue the mass-erase and reboot the node.
 * ! It does not care about anything else in the system!
 * !!!!!!!!!!!!!!!
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#include "devp_flash_mass_erase.h"

#include "cmsis_os2_ext.h"

#include "platform.h"
#include "watchdog.h"
#include "spi_flash.h"

#include <string.h>

static int dp_flash_mass_erase_get(devp_t * param, void * value);
static int dp_flash_mass_erase_set(devp_t * param, bool init, const void * value, uint8_t size);

static devp_t m_dp_flash_mass_erase = {
	.name = "flash_mass_erase",
	.type = DP_TYPE_RAW,
	.size = sizeof(uint8_t),
	.persist = false,
	.getf = dp_flash_mass_erase_get,
	.setf = dp_flash_mass_erase_set
};

static osThreadId_t m_erase_thread = NULL;


static void erase_flash_thread (void * arg)
{
	// Blink LEDs for visual confirmation
	for (int i=0; i<25; i++)
	{
		PLATFORM_LedsSet(1);
		osDelay(200);
		PLATFORM_LedsSet(0);
		osDelay(200);
	}

	spi_flash_lock(); // Acquire the flash, before locking the kernel

	watchdog_enable(240000); // Mass-erase is fairly slow, try 4 minutes

	PLATFORM_LedsSet(1);
	spi_flash_mass_erase();
	PLATFORM_LedsSet(0);

	watchdog_feed();
	watchdog_enable(10000); // blink for 10 seconds
	for (;;) // Wait for watchdog to time out, blinking aggressively
	{
		uint32_t start = osCounterGetMilli();
		while (osCounterGetMilli() - start < 100);
		PLATFORM_LedsSet(PLATFORM_LedsGet() ^ 1);
	}
}

static int dp_flash_mass_erase_get (devp_t * param, void * value)
{
	uint8_t v = 0;
	if (NULL != m_erase_thread)
	{
		v = FLASH_MASS_ERASE_MAGIC_TOKEN;
	}
	memcpy(value, &v, sizeof(uint8_t));
	return sizeof(uint8_t);
}

static int dp_flash_mass_erase_set (devp_t * param, bool init, const void * value, uint8_t size)
{
	if (sizeof(uint8_t) != size)
	{
		return DEVP_ESIZE;
	}

	if (NULL != m_erase_thread)
	{
		return DEVP_EBUSY;
	}

	uint8_t v;
	memcpy(&v, value, size);
	if (FLASH_MASS_ERASE_MAGIC_TOKEN != v)
	{
		return DEVP_EVALUE;
	}

	const osThreadAttr_t erase_thread_attr = { .name = "fe", .stack_size = 1024, .priority = osPriorityRealtime7 };
	m_erase_thread = osThreadNew(erase_flash_thread, NULL, &erase_thread_attr);
	if (NULL != m_erase_thread)
	{
		return sizeof(uint8_t);
	}

	return DEVP_EFAIL;
}

void devp_flash_mass_erase_init ()
{
	devp_register(&m_dp_flash_mass_erase);
}
