/**
 * DeviceParameters local API.
 *
 * Copyright Thinnect Inc. 2020
 * @author Raido Pahtma
 * @license MIT
 **/
#ifndef DEVP_H_
#define DEVP_H_

#include <stdint.h>
#include <stdbool.h>

#include "devp_protocol.h"

typedef enum DevpErrors {
	DEVP_UNKNOWN = -1,
	DEVP_ESIZE = -2,
	DEVP_EREADONLY = -3,
	DEVP_EOFF = -4,
	DEVP_EBUSY = -5,
	DEVP_ETYPE = -6
} devp_errors_t;

typedef struct devp devp_t;

/**
 * Parameter GET function.
 *
 * @param param - The previously registered parameter being accessed.
 * @param set - true when SET, faslse when GET.
 * @param value - pointer to the value according to the registered type and length.
 * @return Negative for errors, (actual)size for success.
 */
typedef int (* devp_get_f)(devp_t * param, void * value);

/**
 * Parameter SET and INIT function.
 *
 * @param param - The registered parameter being accessed.
 * @param init - true when called during the devp_register call.
 * @param value - pointer to the value according to the registered type and length.
 * @return 0 for success.
 */
typedef int (* devp_set_f)(devp_t * param, bool init, void * value, uint8_t size);

/**
 * A generic callback type to notify other modules about changes. Value must be
 * queried separately.
 *
 * @param name - Name of the parameter that changed.
 * @param user - User pointer provided during callback registration.
 */
typedef void (* devp_changed_cb_f)(const char * name, void * user);

/**
 * Initialize module. Make sure to call this immediately after the kernel is
 * started and before initializing any components that register parameters.
 */
void devp_init();

/**
 * Perform garbage collection on the stored variables. Do this after all
 * variables have been registered, it will discard any stored variables it
 * does not recognize.
 */
void devp_gc();

// -----------------------------------------------------------------------------
struct devp
{
	const char * name;     // Name of the parameter, must be unique. Max 16 characters + \0.
	const DeviceParameterTypes_t type; // Type of the parameter.
	const uint8_t size;    // Size of the parameter, must match type size when fixed-size, maximum for variable size.
	const devp_get_f getf; // Pointer to GET function
	const devp_set_f setf; // Pointer to SET function
	const bool persist;    // Value should be placed in persistent storage when changed

	devp_t * next; // devp makes a linked-list of registered parameters
};

/**
 * Register a parameter.
 *
 * It will call the set function if a value is found in persistent storage.
 *
 * @param param - Initialized memory for parameter, must remain available and
 *                must not be modified directly after initialization.
 */
int devp_register(devp_t * param);
// -----------------------------------------------------------------------------


int devp_set(const char * name, DeviceParameterTypes_t type, void * value, uint8_t size);
int devp_get(const char * name, DeviceParameterTypes_t type, void * value, uint8_t size);

int devp_discover_name(const char * name, uint8_t * idx, DeviceParameterTypes_t * type, void * value, uint8_t size);
int devp_discover_idx(uint8_t idx, const char ** name, DeviceParameterTypes_t * type, void * value, uint8_t size);

#endif//DEVP_H_