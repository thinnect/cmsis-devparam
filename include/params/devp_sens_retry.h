/**
 * Generic sensing retry parameter.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_SENS_RETRY_H_
#define DEVP_SENS_RETRY_H_

#include "devp.h"

/**
 * Initialize retry parameter.
 * @param default_retry Default retry count.
 */
void devp_sens_retry_init(uint16_t default_retry);

/**
 * Get sensing retry count.
 *
 * @return Retry count.
 */
uint16_t devp_sens_retry_get();

#endif//DEVP_SENS_RETRY_H_
