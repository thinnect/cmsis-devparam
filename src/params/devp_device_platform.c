/**
 * Device platform type-version through DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */

#include "devp_device_platform.h"

#include <string.h>

#include "DeviceSignature.h"

#include "sm_version.h"

static int dp_device_platform_get (devp_t * param, void * value);

static devp_t m_dp_device_platform =
{
    .name = "device_platform",
    .type = DP_TYPE_STRING,
    .size = PLATFORM_VERSION_SIZE,
    .persist = false,
    .getf = dp_device_platform_get,
    .setf = NULL
};

int dp_device_platform_get (devp_t * param, void * value)
{
    uint8_t platform_uuid[16];
    sigGetPlatformUUID(platform_uuid);

    char smnt_platform_version[PLATFORM_VERSION_SIZE];
    sm_version_get_platform_text(platform_uuid, smnt_platform_version);

    strcpy((char*)value, smnt_platform_version);
    
    return PLATFORM_VERSION_SIZE;
}

void devp_device_platform_init ()
{
    devp_register(&m_dp_device_platform);
}