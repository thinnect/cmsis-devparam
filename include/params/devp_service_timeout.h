/**
 * Service mode timeout for deviceparameter.
 * Every time when deviceparameter is used devp_comms uses timer to keep radio alive for some time. Default is 5 seconds.
 * This timeout can be changed via this devparam.
 *
 * Copyright Thinnect Inc. 2022
 * @author Lembitu Valdmets
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
