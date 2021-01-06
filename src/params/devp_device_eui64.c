/**
 * Device EUI64 through DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */

#include "devp_device_eui64.h"

#include <string.h>

#include "DeviceSignature.h"

static int dp_device_eui64_get (devp_t * param, void * value);

static devp_t m_dp_device_eui64 =
{
    .name = "eui64",
    .type = DP_TYPE_RAW,
    .size = IEEE_EUI64_LENGTH,
    .persist = false,
    .getf = dp_device_eui64_get,
    .setf = NULL
};

int dp_device_eui64_get (devp_t * param, void * value)
{
    sigGetEui64(value);
    return IEEE_EUI64_LENGTH;
}

void devp_device_eui64_init ()
{
    devp_register(&m_dp_device_eui64);
}
