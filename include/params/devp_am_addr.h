/**
 * ActiveMessage address parameter.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_AM_ADDR_H_
#define DEVP_AM_ADDR_H_

#include "devp.h"

/**
 * Initialize AM addr parameter.
 */
void devp_am_addr_init (void);

/**
 * Get address.
 *
 * @return AM address.
 */
uint16_t devp_am_addr_get (void);

#endif//DEVP_AM_ADDR_H_
