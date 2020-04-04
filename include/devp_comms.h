/**
 * DeviceParameters remote access module.
 *
 * Copyright Thinnect Inc. 2020
 * @author Raido Pahtma
 * @license MIT
 **/
#ifndef DEVP_COMMS_H_
#define DEVP_COMMS_H_

#include <stdint.h>
#include <stdbool.h>

#include "devp_protocol.h"

#include "mist_comm.h"

void devp_comms_init();

int devp_add_iface(comms_layer_t * comms, bool heartbeat);
int devp_remove_iface(comms_layer_t * comms);

#endif//DEVP_COMMS_H_
