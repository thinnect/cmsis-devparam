
#include <string.h>

#include "devp.h"

static devp_t * mp_params;

void devp_init ()
{
	mp_params = NULL;
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

	// TODO search persistent storage for initialization value

	return 0;
}

static devp_t * _devp_find(const char * name)
{
	// Search
	for (devp_t ** ppd = &(mp_params); *ppd != NULL; ppd = &((*ppd)->next))
	{
		if (0 == strcmp((*ppd)->name, name))
		{
			return *ppd;
		}
	}
	return NULL;
}

int devp_set(const char * name, DeviceParameterTypes_t type, void * value, uint8_t size)
{
	devp_t * pparam = _devp_find(name);
	if (NULL == pparam)
	{
		return -1;
	}
	if (pparam->type != type)
	{
		return -2;
	}

	if (NULL == pparam->setf) // Read-Only
	{
		return -5;
	}

	// Set
	if (0 != pparam->setf(pparam, false, value, size))
	{
		return -3;
	}

	// Persist
	if (pparam->persist)
	{
		// TODO
	}

	return 0;
}

int devp_get(const char * name, DeviceParameterTypes_t type, void * value, uint8_t size)
{
	devp_t * pparam = _devp_find(name);
	if (NULL == pparam)
	{
		return -1;
	}
	if (pparam->type != type)
	{
		return -2;
	}
	if (pparam->size > size) // Disregarding actual size for variable size parameters
	{
		return -4;
	}
	return pparam->getf(pparam, value);
}

int devp_discover_name(const char * name, DeviceParameterTypes_t * type, void * value, uint8_t size)
{
	devp_t * pparam = _devp_find(name);
	if (NULL == pparam)
	{
		return -1;
	}
	if (pparam->size > size) // Disregarding actual size for variable size parameters
	{
		return -4;
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
			return -1;
		}
		if(i < idx)
		{
			pparam = pparam->next;
		}
	}

	if(pparam->size > size)
	{
		return -4;
	}

	*name = pparam->name;
	*type = pparam->type;
	return pparam->getf(pparam, value);
}