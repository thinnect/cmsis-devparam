/**
 * SCD30/4x default value for automatic self calibration.
 *
 * Copyright Thinnect Inc. 2021
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
 * Get SCD ASC default value.
 *
 * @return default value for ASC.
 */
uint8_t devp_scd_default_asc_get();

#endif//DEVP_SCD_DEFAULT_ASC_H_
