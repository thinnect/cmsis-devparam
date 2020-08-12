/**
 * UNIX time parameter for getting/setting node time.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_UNIX_TIME_H_
#define DEVP_UNIX_TIME_H_

#include "devp.h"

typedef void (*devp_unix_time_changed_f) ();

/**
 * Initialize UNIX time parameter.
 */
void devp_unix_time_init(devp_unix_time_changed_f changed);

#endif//DEVP_UNIX_TIME_H_
