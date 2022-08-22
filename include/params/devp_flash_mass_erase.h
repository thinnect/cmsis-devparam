/**
 * Force a dataflash mass-erase over DeviceParameters.
 *
 * If the `flash_mass_erase` parameter is set to the value of the
 * "magic" token, a mass-erase will be performed, the device will probably
 * forget all or at least most of its configuration and reboot.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_FLASH_MASS_ERASE_H_
#define DEVP_FLASH_MASS_ERASE_H_

#define FLASH_MASS_ERASE_MAGIC_TOKEN 0xFE

#include "devp.h"

void devp_flash_mass_erase_init();

#endif//DEVP_FLASH_MASS_ERASE_H_
