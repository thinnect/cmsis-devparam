/**
 * Generic sensing period parameter.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_SENS_PERIOD_H_
#define DEVP_SENS_PERIOD_H_

#include "devp.h"

/**
 * Initialize period parameter.
 * @param default_period Default sensing period, seconds.
 */
void devp_sens_period_init(uint16_t default_period);

/**
 * Get sensing period.
 *
 * @return Sensing period in seconds.
 */
uint16_t devp_sens_period_get();

#endif//DEVP_SENS_PERIOD_H_
