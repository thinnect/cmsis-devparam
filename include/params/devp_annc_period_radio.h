/**
 * DeviceParameter for device announcement period.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#ifndef DEVP_ANNC_PERIOD_RADIO_H
#define DEVP_ANNC_PERIOD_RADIO_H

#include "devp.h"

/**
 * Initialize announcement period parameter.
 */
void devp_annc_period_radio_init (uint32_t default_period_s, uint32_t min_period_s);

/**
 * Get announcement period.
 *
 * @return Announcement period in seconds.
 */
uint32_t devp_annc_period_radio_get (void);

#endif//DEVP_ANNC_PERIOD_RADIO_H
