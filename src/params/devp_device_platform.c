/**
 * Device platform type-version through DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */

#include "devp_device_platform.h"

#include <string.h>

#include "DeviceSignature.h"

#define PLATFORM_NAME_SIZE 16

static int dp_device_platform_get (devp_t * param, void * value);

static devp_t m_dp_device_platform =
{
    .name = "device_platform",
    .type = DP_TYPE_STRING,
    .size = PLATFORM_NAME_SIZE,
    .persist = false,
    .getf = dp_device_platform_get,
    .setf = NULL
};

int dp_device_platform_get (devp_t * param, void * value)
{
    char platform_name[PLATFORM_NAME_SIZE];
    sigGetPlatformName(platform_name, PLATFORM_NAME_SIZE);

    strcpy((char*)value, platform_name);
    
    return PLATFORM_NAME_SIZE;
}

void devp_device_platform_init ()
{
    devp_register(&m_dp_device_platform);
}
