# Types and sizes

The APIs use a *void* pointer and a value type. The storage behind the pointer
is always assumed to be of the correct size - the size provided during
registration. For variable-size parameters, it is the maximum size of the value.

# Shared access

TODO - other modules should be able to query for values, maybe set them.

Actual run-time value of the parameter is handled by the module that declared
it, but the new DEVP API also allows values to be shared between components.
Simply ask for the value by name, type and size - the type needs to match up,
and size needs to be big enough, otherwise an error is returned.