/**
 * DeviceParameters storage dummy implementation.
 *
 * Copyright Thinnect Inc. 2020
 * @author Raido Pahtma
 * @license MIT
 **/
#include "devp_storage.h"
#include "devp.h"

#include "loglevels.h"
#define __MODUUL__ "dps"
#define __LOG_LEVEL__ (LOG_LEVEL_devp_storage & BASE_LOG_LEVEL)
#include "log.h"

bool devp_storage_init(devp_storage_t * strg)
{
	warn1("storage dummy");
	return true;
}

int devp_storage_load(devp_storage_t * strg, const char * name, void * value, uint8_t length)
{
	return DEVP_UNKNOWN;
}

int devp_storage_save(devp_storage_t * strg, const char * name, const void * value, uint8_t length, uint8_t maxlength)
{
	return length;
}
