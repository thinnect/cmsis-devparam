/**
 * DeviceParamater parameter and storage handler.
 *
 * Copyright Thinnect Inc. 2020
 * @author Raido Pahtma
 * @license MIT
 */

#include <string.h>

#include "devp.h"
#include "devp_storage.h"

static devp_t * mp_params;

void devp_init ()
{
	mp_params = NULL;
	devp_storage_init(NULL);
}

void devp_gc ()
{
	// TODO
}

int devp_register(devp_t * param)
{
	devp_t ** ppd = &(mp_params);
	while(*ppd != NULL)
	{
		ppd = &((*ppd)->next);
	}
	*ppd = param;

	param->next = NULL;

	if (param->persist)
	{
		uint8_t value[param->size];
		int len = devp_storage_load(NULL, param->name, value, sizeof(value));
		if (len >= 0) // TODO actual length check
		{
			param->setf(param, true, value, len);
		}
	}

	return 0;
}

static devp_t * _devp_find(const char * name, uint8_t * idx)
{
	// Search
	uint8_t i = 0;
	for (devp_t ** ppd = &(mp_params); *ppd != NULL; ppd = &((*ppd)->next))
	{
		if (0 == strcmp((*ppd)->name, name))
		{
			if(NULL != idx)
			{
				*idx = i;
			}
			return *ppd;
		}
		i++;
	}
	return NULL;
}

int devp_set(const char * name, DeviceParameterTypes_t type, void * value, uint8_t size)
{
	devp_t * pparam = _devp_find(name, NULL);
	if (NULL == pparam)
	{
		return DEVP_UNKNOWN;
	}
	if (pparam->type != type)
	{
		return DEVP_ETYPE;
	}

	if (NULL == pparam->setf) // Read-Only
	{
		return DEVP_EREADONLY;
	}

	// Set
	int ret = pparam->setf(pparam, false, value, size);
	if (0 > ret)
	{
		return ret;
	}

	// Persist
	if (pparam->persist)
	{
		int len = devp_storage_save(NULL, pparam->name, value, size, pparam->size);
		return len;
	}

	return 0;
}

int devp_get(const char * name, DeviceParameterTypes_t type, void * value, uint8_t size)
{
	devp_t * pparam = _devp_find(name, NULL);
	if (NULL == pparam)
	{
		return DEVP_UNKNOWN;
	}
	if (pparam->type != type)
	{
		return DEVP_ETYPE;
	}
	if (pparam->size > size) // Disregarding actual size for variable size parameters
	{
		return DEVP_ESIZE;
	}
	return pparam->getf(pparam, value);
}

int devp_discover_name(const char * name, uint8_t * idx, DeviceParameterTypes_t * type, void * value, uint8_t size)
{
	devp_t * pparam = _devp_find(name, idx);
	if (NULL == pparam)
	{
		return DEVP_UNKNOWN;
	}
	if (pparam->size > size) // Disregarding actual size for variable size parameters
	{
		return DEVP_ESIZE;
	}
	*type = pparam->type;
	return pparam->getf(pparam, value);
}

int devp_discover_idx(uint8_t idx, const char ** name, DeviceParameterTypes_t * type, void * value, uint8_t size)
{
	devp_t * pparam = mp_params;
	for(uint8_t i=0;i<=idx;i++)
	{
		if(NULL == pparam)
		{
			return DEVP_UNKNOWN;
		}
		if(i < idx)
		{
			pparam = pparam->next;
		}
	}

	if(pparam->size > size)
	{
		return DEVP_ESIZE;
	}

	*name = pparam->name;
	*type = pparam->type;
	return pparam->getf(pparam, value);
}