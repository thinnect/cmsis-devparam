/**
 * DeviceParamater light sensor status and configuration.
 *
 * Copyright Thinnect Inc. 2023
 * @license MIT
 */
#ifndef DEVP_LIGHTSENS_STATUS_H_
#define DEVP_LIGHTSENS_STATUS_H_

#include <stdint.h>

enum DPLightsensStatus
{
	LIGHT_SENSOR_ERROR         = -4, // Something is wrong
	LIGHT_SENSOR_OVERRIDDEN    = -3, // Alternative sensor is used
	LIGHT_SENSOR_UNAVAILABLE   = -2, // Sensor not detected
	LIGHT_SENSOR_DISABLED      = -1, // Forcefully disabled via DP, may or may not be available
	LIGHT_SENSOR_UNINITIALIZED = 0,  // Nothing has happened yet with the sensor
	LIGHT_SENSOR_ENABLED       = 1,  // Detected and enabled
};

void devp_lightsens_status_init ();

/**
 * Check if light sensor is enabled and detected.
 * @return one of the enum DPLightsensStatus values.
 */
enum DPLightsensStatus devp_lightsens_status_get ();

/**
 * Update light sensor status.
 * @param status New status.
 */
void devp_lightsens_status_set (enum DPLightsensStatus status);

#endif//DEVP_LIGHTSENS_STATUS_H_
