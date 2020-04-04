/**
 * Default PAN ID parameter.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_PAN_ID_H_
#define DEVP_PAN_ID_H_

#include "devp.h"

/**
 * Initialize PAN ID parameter.
 */
void devp_pan_id_init();

/**
 * Get address.
 *
 * @return PAN ID.
 */
uint16_t devp_pan_id_get();

#endif//DEVP_PAN_ID_H_
