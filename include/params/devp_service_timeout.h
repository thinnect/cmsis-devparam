/**
 * SCD30/4x default value for automatic self calibration.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#ifndef DEVP_SERVICE_MODE_H_
#define DEVP_SERVICE_MODE_H_

#include "devp.h"

/**
 * Initialize scd default automatic self calibration parameter.
 */
void devp_service_mode_timeout_init ();

/**
 * Get current timeout value in milliseconds.
 *
 * @return default value for ASC.
 */
uint32_t devp_service_mode_timeout_get ();

#endif//DEVP_SERVICE_MODE_H_
