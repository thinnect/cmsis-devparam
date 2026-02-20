/**
 * Bootloader info through deviceparameters.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#ifndef DEVP_BOOTLOADER_H_
#define DEVP_BOOTLOADER_H_

#include <stdint.h>

void devp_bootloader_init ();
int devp_bootloader_get (char* version_str);

#endif//DEVP_BOOTLOADER_H_
