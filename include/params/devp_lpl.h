/**
 * Active Message address parameter.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_LPL_H_
#define DEVP_LPL_H_

#include "devp.h"

/**
 * Initialize LowPowerListening parameters.
 */
void devp_lpl_init();

/**
 * Get lpl_remote_wakeup.
 *
 * @return lpl_remote_wakeup.
 */
uint16_t devp_lpl_remote_wakeup_get();

#endif//DEVP_LPL_H_
