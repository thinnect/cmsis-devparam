#ifndef DEVICE_MODES_H_
#define DEVICE_MODES_H_

typedef enum device_modes {
	DEVICE_MODE_GATEWAY,
	DEVICE_MODE_SNIFFER,
	DEVICE_MODE_CONNECTOR,
	DEVICE_MODE_TOTAL_COUNT
} device_modes_t;

#define DEVICE_MODE_STRINGS "gateway","sniffer","connector"

// Longest string in this case is connector
#define DEVICE_MODE_STRINGS_MAX_LENGTH (sizeof("connector")-1)


#if defined(DEFAULT_DEVICE_MODE_SNIFFER)
	#pragma message("DEVICE_MODE_SNIFFER")
	#define DEFAULT_DEVICE_MODE DEVICE_MODE_SNIFFER
#elif defined(DEFAULT_DEVICE_MODE_CONNECTOR)
	#pragma message("DEVICE_MODE_CONNECTOR")
	#define DEFAULT_DEVICE_MODE DEVICE_MODE_CONNECTOR
#else
	#define DEFAULT_DEVICE_MODE DEVICE_MODE_GATEWAY
#endif

#endif//DEVICE_MODES_H_
