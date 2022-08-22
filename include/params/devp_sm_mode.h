/**
 * DeviceParamater SMENETE mode selection options.
 *
 * Copyright Thinnect Inc. 2022
 * @license MIT
 */
#ifndef DEVP_SM_MODE_H_
#define DEVP_SM_MODE_H_

#include "devp.h"

void devp_sm_mode_init ();

uint8_t get_sm_mode ();

void set_sm_mode (uint8_t sm_mode);

const char* get_sm_mode_string ();

#endif//DEVP_SM_MODE_H_
