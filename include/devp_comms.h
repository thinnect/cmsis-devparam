/**
 * DeviceParameters remote access module.
 *
 * Copyright Thinnect Inc. 2020
 * @author Raido Pahtma
 * @license MIT
 **/
#ifndef DEVP_COMMS_H
#define DEVP_COMMS_H

#include <stdint.h>
#include <stdbool.h>

#include "devp_protocol.h"

#include "mist_comm.h"
#include "mist_comm_pool.h"

/**
 * Initialize DeviceParameters communications.
 * @param p_pool - A mist-comm message pool.
 *
 * @return true If successfully initialized and task created.
 */
bool devp_comms_init (comms_pool_t * p_pool);

int devp_add_iface(comms_layer_t * comms, comms_sleep_controller_t * ctrl, bool heartbeat);

int devp_remove_iface(comms_layer_t * comms);

#endif//DEVP_COMMS_H
