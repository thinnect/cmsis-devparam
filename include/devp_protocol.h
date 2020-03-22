/**
 * DeviceParameters protocol.
 *
 * Copyright Thinnect Inc. 2020
 * @author Raido Pahtma
 * @license MIT
 **/
#ifndef DEVP_PROTOCOL_H_
#define DEVP_PROTOCOL_H_

	#include "eui64.h"

	#ifndef DEVICE_PARAMETERS_SERIAL_DISPATCHER_ID
 	#define DEVICE_PARAMETERS_SERIAL_DISPATCHER_ID 0x80
 	#endif // DEVICE_PARAMETERS_SERIAL_DISPATCHER_ID

	#ifndef AMID_DEVICE_PARAMETERS
	#define AMID_DEVICE_PARAMETERS 0x82
	#endif // AMID_DEVICE_PARAMETERS

 	#ifndef DP_HEARTBEAT_PERIOD_S
 	#define DP_HEARTBEAT_PERIOD_S 0
 	#endif // DP_HEARTBEAT_PERIOD_S

	enum DeviceParametersHeader {
		DP_HEARTBEAT = 0x00,

		DP_PARAMETER = 0x10,

		DP_GET_PARAMETER_WITH_ID     = 0x21,
		DP_GET_PARAMETER_WITH_SEQNUM = 0x22,

		DP_SET_PARAMETER_WITH_ID     = 0x31,
		DP_SET_PARAMETER_WITH_SEQNUM = 0x32,

		DP_ERROR_PARAMETER_ID     = 0xF0,
		DP_ERROR_PARAMETER_SEQNUM = 0xF1
	};

	typedef enum DeviceParameterTypes {
		DP_TYPE_RAW     = 0x00, // 00000000

		DP_TYPE_UINT8   = 0x01, // 00000001
		DP_TYPE_UINT16  = 0x02, // 00000010
		DP_TYPE_UINT32  = 0x04, // 00000100
		DP_TYPE_UINT64  = 0x08, // 00001000

		DP_ARRAY_UINT8  = 0x11, // 00010001
		DP_ARRAY_UINT16 = 0x12, // 00010010
		DP_ARRAY_UINT32 = 0x14, // 00010100
		DP_ARRAY_UINT64 = 0x18, // 00011000

		DP_TYPE_STRING  = 0x80, // 10000000

		DP_TYPE_INT8    = 0x81, // 10000001
		DP_TYPE_INT16   = 0x82, // 10000010
		DP_TYPE_INT32   = 0x84, // 10000100
		DP_TYPE_INT64   = 0x88, // 10001000

		DP_ARRAY_INT8   = 0x91, // 10010001
		DP_ARRAY_INT16  = 0x92, // 10010010
		DP_ARRAY_INT32  = 0x94, // 10010100
		DP_ARRAY_INT64  = 0x98, // 10011000

		DP_TYPE_BOOL    = 0xFF, // 11111111
	} DeviceParameterTypes_t;

	#pragma pack(push, 1)
	typedef struct dp_heartbeat {
		uint8_t header;
		uint8_t eui64[IEEE_EUI64_LENGTH]; // device EUI64
		uint32_t uptime; // seconds
	} dp_heartbeat_t;
	#pragma pack(pop)

	#pragma pack(push, 1)
	typedef struct dp_parameter {
		uint8_t header; // DP_FEATURE
		uint8_t type;   // DeviceParameterTypes
		uint8_t seqnum;
		uint8_t idlength;
		uint8_t valuelength;
		char id[];
		//uint8_t value[];
	} dp_parameter_t;
	#pragma pack(pop)

	#pragma pack(push, 1)
	typedef struct dp_get_parameter_seqnum {
		uint8_t header;
		uint8_t seqnum;
	} dp_get_parameter_seqnum_t;
	#pragma pack(pop)

	#pragma pack(push, 1)
	typedef struct dp_get_parameter_id {
		uint8_t header;
		uint8_t idlength;
		char id[];
	} dp_get_parameter_id_t;
	#pragma pack(pop)

	#pragma pack(push, 1)
	typedef struct dp_set_parameter_seqnum {
		uint8_t header;
		uint8_t seqnum;
		uint8_t valuelength;
		//uint8_t value[];
	} dp_set_parameter_seqnum_t;
	#pragma pack(pop)

	#pragma pack(push, 1)
	typedef struct dp_set_parameter_id {
		uint8_t header;
		uint8_t idlength;
		uint8_t valuelength;
		char id[];
		//uint8_t value[];
	} dp_set_parameter_id_t;
	#pragma pack(pop)

	#pragma pack(push, 1)
	typedef struct dp_error_parameter_seqnum {
		uint8_t header;
		uint8_t exists;
		uint8_t error;
		uint8_t seqnum;
	} dp_error_parameter_seqnum_t;
	#pragma pack(pop)

	#pragma pack(push, 1)
	typedef struct dp_error_parameter_id {
		uint8_t header;
		uint8_t exists;
		uint8_t error;
		uint8_t idlength;
		char id[];
	} dp_error_parameter_id_t;
	#pragma pack(pop)

#endif // DEVP_PROTOCOL_H_
