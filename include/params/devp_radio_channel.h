/**
 * DeviceParamater radio channel selection options.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */
#ifndef DEVP_RADIO_CHANNEL_H_
#define DEVP_RADIO_CHANNEL_H_

#include "devp.h"

typedef uint8_t (* get_radio_channel_f)();

void devp_radio_channel_init(get_radio_channel_f gcurrent, devp_changed_cb_f callback, void * user);

uint8_t get_radio_ch_default();
uint8_t get_radio_channel();

#endif//DEVP_RADIO_CHANNEL_H_
