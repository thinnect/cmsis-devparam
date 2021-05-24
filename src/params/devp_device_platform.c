/**
 * Device platform type-version through DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */

#include "devp_device_platform.h"

#include <string.h>
#include <stdio.h>

#include "DeviceSignature.h"

#define PLATFORM_NAME_SIZE 17
#define PLATFORM_VERSION_SIZE 12
#define PLATFORM_UUID_SIZE 16

static int dp_device_platform_name_get (devp_t * param, void * value);
static int dp_device_platform_version_get (devp_t * param, void * value);
static int dp_device_platform_uuid_get (devp_t * param, void * value);

static devp_t m_dp_device_platform_name =
{
    .name = "platform_name",
    .type = DP_TYPE_STRING,
    .size = PLATFORM_NAME_SIZE,
    .persist = false,
    .getf = dp_device_platform_name_get,
    .setf = NULL
};

static devp_t m_dp_device_platform_version =
{
    .name = "platform_version",
    .type = DP_TYPE_STRING,
    .size = PLATFORM_VERSION_SIZE,
    .persist = false,
    .getf = dp_device_platform_version_get,
    .setf = NULL
};

static devp_t m_dp_device_platform_uuid =
{
    .name = "platform_uuid",
    .type = DP_TYPE_RAW,
    .size = PLATFORM_UUID_SIZE,
    .persist = false,
    .getf = dp_device_platform_uuid_get,
    .setf = NULL
};

int dp_device_platform_name_get (devp_t * param, void * value)
{
    sigGetPlatformName(value, PLATFORM_NAME_SIZE);
    return strlen(value);
}

int dp_device_platform_version_get (devp_t * param, void * value)
{
    semver_t ver = sigGetPlatformVersion();
    int len = snprintf(value, PLATFORM_VERSION_SIZE, "%d.%d.%d", ver.major, ver.minor, ver.patch);
    if (len < PLATFORM_VERSION_SIZE)
    {
        return len;
    }
    return PLATFORM_VERSION_SIZE - 1;
}

int dp_device_platform_uuid_get (devp_t * param, void * value)
{
    sigGetPlatformUUID(value);
    return PLATFORM_UUID_SIZE;
}

void devp_device_platform_init ()
{
    devp_register(&m_dp_device_platform_name);
    devp_register(&m_dp_device_platform_version);
    devp_register(&m_dp_device_platform_uuid);
}
