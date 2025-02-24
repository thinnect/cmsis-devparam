/**
 * DeviceParameters storage API.
 *
 * Copyright Thinnect Inc. 2020
 * @author Raido Pahtma
 * @license MIT
 **/
#ifndef DEVP_STORAGE_H_
#define DEVP_STORAGE_H_

#ifndef DEVP_STORAGE_FS_ID
#define DEVP_STORAGE_FS_ID 0
#endif//DEVP_STORAGE_FS_ID

#ifndef DEVP_STORAGE_FNAME
#define DEVP_STORAGE_FNAME "devp1"
#endif//DEVP_STORAGE_FNAME

#include <stdbool.h>
#include <stdint.h>

typedef struct devp_storage
{
	int32_t f;
} devp_storage_t;

bool devp_storage_init(devp_storage_t * strg);
int devp_storage_load(devp_storage_t * strg, const char * name, void * value, uint8_t length);
int devp_storage_save(devp_storage_t * strg, const char * name, const void * value, uint8_t length, uint8_t maxlength);

#endif//DEVP_STORAGE_H_
