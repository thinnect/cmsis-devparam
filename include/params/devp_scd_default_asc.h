/**
 * Active Message address parameter.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_SCD_DEFAULT_ASC_H_
#define DEVP_SCD_DEFAULT_ASC_H_

#include "devp.h"

/**
 * Initialize scd default automatic self calibration parameter.
 */
void devp_scd_default_asc_init();

/**
 * Get default_scd.
 *
 * @return AM address.
 */
uint8_t devp_scd_default_asc_get();

#endif//DEVP_SCD_DEFAULT_ASC_H_
