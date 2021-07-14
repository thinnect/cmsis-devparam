/**
 * Bootloader info through deviceparameters.
 *
 * MAJOR.MINOR.PATCH.DEV-IDENT_TIMESTAMP
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#include "devp_bootloader.h"

#include "header.h"

#include "devp.h"

#include <stdio.h>
#include <inttypes.h>

#include "loglevels.h"
#define __MODUUL__ "dbtldr"
#define __LOG_LEVEL__ (LOG_LEVEL_devp_bootloader & BASE_LOG_LEVEL)
#include "log.h"

#define BOOTLOADER_TIMESTAMP_MAX_STRING_LENGTH 16

/**
 * Read directly from mcu flash, will work on platforms where flash is at addr 0.
 * FIXME: Use a generic platform function ... when one is added to node-platform.
 */
static uint8_t flash_read_mcu (uint32_t ptr)
{
	return(*(uint8_t *)ptr);
}

static int dp_bootloader_get(devp_t * param, void * value);

static devp_t m_dp_bootloader = {
	.name = "bootloader",
	.type = DP_TYPE_STRING,
	.size = 40, // 000.000.000.dev000-0011223344556677 with some room to spare
	.persist = false,
	.getf = dp_bootloader_get,
	.setf = NULL
};

static int dp_bootloader_get (devp_t * param, void * value)
{
	uint32_t header_bootloader = header_find(BOOTLOADER_START, BOOTLOADER_START + BOOTLOADER_MAX_LEN, flash_read_mcu);
	if (0 == header_bootloader)
	{
		return DEVP_EVALUE;
	}

	int param_len = 0;

	// version is a string
	uint32_t ver_len = 0;
	uint32_t ver_addr = header_get(header_bootloader, HEADER_VALUE_ID_VERSION, 1, &ver_len, flash_read_mcu);
	if (0 != ver_addr)
	{
		if (ver_len < m_dp_bootloader.size)
		{
			for (uint32_t i = 0; i < ver_len; i++)
			{
				((uint8_t*)value)[i] = flash_read_mcu(ver_addr + i);
			}
			param_len = ver_len;
			((char*)value)[param_len++] = '-';
		}
	}

	// timestamp is uint64, we will format it as hex
	uint32_t ts_len = 0;
	uint32_t ts_addr = header_get(header_bootloader, HEADER_VALUE_TIMESTAMP, sizeof(uint64_t), &ts_len, flash_read_mcu);
	if ((0 != ts_addr)&&(sizeof(uint64_t) == ts_len))
	{
		// Format timestamp only if we have enough space left
		int free_space = m_dp_bootloader.size - param_len;
		if (free_space > BOOTLOADER_TIMESTAMP_MAX_STRING_LENGTH) // accounts for \0
		{
			uint64_t ts = header_u64(ts_addr, flash_read_mcu);
			int tlen = sprintf(&((char*)value)[param_len], "%"PRIX64, ts);
			if (tlen > 0)
			{
				param_len += tlen;
			}
			else
			{
				warn1("l: %d", tlen);
			}
		}
		else
		{
			warn1("l: %d", free_space);
		}
	}
	else
	{
		warn1("a:%"PRIx32" l:%"PRIu32, ts_addr, ts_len);
	}

	return param_len;
}

void devp_bootloader_init ()
{
	devp_register(&m_dp_bootloader);
}
