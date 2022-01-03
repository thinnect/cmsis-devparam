/**
 * SCD4X service mode parameters.
 *
 * Copyright Thinnect Inc. 2021
 * @license MIT
 */
#ifndef DEVP_SCD4X_SERVICE_MODE_H_
#define DEVP_SCD4X_SERVICE_MODE_H_

#include "devp.h"

/**
 * Initialize SCD4X service mode parameters.
 */
void devp_scd4x_service_mode_init ();

/**
* Initialize SCD4X default tempoffset
*/
void dp_scd4x_tempoffs_default_init ();

uint32_t devp_scd4x_tempoffs_default_get ();

#endif//DEVP_SCD4X_SERVICE_MODE_H_
