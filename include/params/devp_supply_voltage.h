/**
 * @file devp_supply_voltage.h
 * @author Lembitu Valdmets
 * @brief Can access to supply voltage, uses sensors.c --> read_battery_universal for this.
 * For usage, initialize it with devp_supply_voltage_init();
 * Will display battery supply voltage in millivolts. Datatype is uint16_t
 * @version 0.1
 * @date 2022-10-14
 * @license priorietary
 *
 */

#ifndef DEVP_SUPPLY_VOLTAGE_H_
#define DEVP_SUPPLY_VOLTAGE_H_

#include "devp.h"

void devp_supply_voltage_init ();

#endif
