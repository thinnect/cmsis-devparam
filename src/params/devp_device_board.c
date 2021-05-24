/**
 * Device board info through DeviceParamaters.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */

#include "devp_device_board.h"

#include <string.h>
#include <stdio.h>

#include "DeviceSignature.h"

#define BOARD_NAME_SIZE 17
#define BOARD_VERSION_SIZE 12
#define BOARD_UUID_SIZE 16

static int dp_device_board_name_get (devp_t * param, void * value);
static int dp_device_board_version_get (devp_t * param, void * value);
static int dp_device_board_uuid_get (devp_t * param, void * value);

static devp_t m_dp_device_board_name =
{
    .name = "board_name",
    .type = DP_TYPE_STRING,
    .size = BOARD_NAME_SIZE,
    .persist = false,
    .getf = dp_device_board_name_get,
    .setf = NULL
};

static devp_t m_dp_device_board_version =
{
    .name = "board_version",
    .type = DP_TYPE_STRING,
    .size = BOARD_VERSION_SIZE,
    .persist = false,
    .getf = dp_device_board_version_get,
    .setf = NULL
};

static devp_t m_dp_device_board_uuid =
{
    .name = "board_uuid",
    .type = DP_TYPE_RAW,
    .size = BOARD_UUID_SIZE,
    .persist = false,
    .getf = dp_device_board_uuid_get,
    .setf = NULL
};

int dp_device_board_name_get (devp_t * param, void * value)
{
    sigGetBoardName(value, BOARD_NAME_SIZE);
    return strlen(value);
}

int dp_device_board_version_get (devp_t * param, void * value)
{
    semver_t ver = sigGetBoardVersion();
    int len = snprintf(value, BOARD_VERSION_SIZE, "%d.%d.%d", ver.major, ver.minor, ver.patch);
    if (len < BOARD_VERSION_SIZE)
    {
        return len;
    }
    return BOARD_VERSION_SIZE - 1;
}

int dp_device_board_uuid_get (devp_t * param, void * value)
{
    sigGetBoardUUID(value);
    return BOARD_UUID_SIZE;
}

void devp_device_board_init ()
{
    devp_register(&m_dp_device_board_name);
    devp_register(&m_dp_device_board_version);
    devp_register(&m_dp_device_board_uuid);
}
