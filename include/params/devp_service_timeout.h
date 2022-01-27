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
 * Initialize default service mode timeout parameter.
 */
void devp_service_mode_timeout_init ();

/**
 * Get current timeout value in milliseconds.
 *
 * @return service mode timeout in milliseconds.
 */
uint32_t devp_service_mode_timeout_get ();

#endif//DEVP_SERVICE_MODE_H_
