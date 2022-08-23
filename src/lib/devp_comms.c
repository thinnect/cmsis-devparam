/**
 * DeviceParamater communications handler.
 *
 * Copyright Thinnect Inc. 2020
 * @license MIT
 */

#include <string.h>

#include "devp.h"
#include "devp_comms.h"
#include "endianness.h"

#ifdef SERVICE_MODE_TIMEOUT
#include "devp_service_timeout.h"
#endif //SERVICE_MODE_TIMEOUT

#include "loglevels.h"
#define __MODUUL__ "dpc"
#define __LOG_LEVEL__ (LOG_LEVEL_devp_comms & BASE_LOG_LEVEL)
#include "log.h"

#define DEVP_FLAGS_RX    (1 << 0)
#define DEVP_FLAGS_SENT  (1 << 1)
#define DEVP_FLAGS_SLEEP (1 << 2)
#define DEVP_FLAGS_ALL   (   0x7)

#ifndef DEVP_MAX_IFACES
#define DEVP_MAX_IFACES 3
#endif//DEVP_MAX_IFACES

#ifndef DEVP_MAX_VALUE_SIZE
#define DEVP_MAX_VALUE_SIZE 64
#endif//DEVP_MAX_VALUE_SIZE

#ifndef DEVP_DEFAULT_SLEEP_TIMEOUT_MS
#define DEVP_DEFAULT_SLEEP_TIMEOUT_MS 30000
#endif//DEVP_DEFAULT_SLEEP_TIMEOUT_MS

static osThreadId_t m_devp_thread_id;
static osTimerId_t m_sleep_timer;
static uint32_t m_rcomm_keepalive_timeout;

static osMutexId_t m_rx_mutex;
static comms_msg_t m_rx_msg; // We keep a dedicated message for RX purpose and do not use the pool
static comms_layer_t * mp_rx_iface;

static comms_pool_t * mp_pool;

static osMutexId_t m_rcvr_mutex;
static comms_layer_t * mp_ifaces[DEVP_MAX_IFACES];
static comms_sleep_controller_t * mp_sleep_ctrl[DEVP_MAX_IFACES];
static bool m_heartbeat[DEVP_MAX_IFACES];
static comms_receiver_t m_rcvrs[DEVP_MAX_IFACES];


static void memcpy_swap (void * dst, void * src, size_t length)
{
	#ifdef __LITTLE_ENDIAN__
		for (int i = 0; i < length; i++)
		{
			((uint8_t*)dst)[i] = ((uint8_t*)src)[length - 1 - i];
		}
	#else
		memcpy(dst, src, length);
	#endif//__LITTLE_ENDIAN__
}


static void radio_status_changed(comms_layer_t * comms, comms_status_t status, void * user)
{
    // Actual status change is checked by polling during startup, but the callback is mandatory
}


static void receive_message (comms_layer_t * comms, const comms_msg_t * msg, void * user)
{
	while (osOK != osMutexAcquire(m_rx_mutex, osWaitForever));
	if (NULL == mp_rx_iface)
	{
		memcpy(&m_rx_msg, msg, sizeof(comms_msg_t));
		mp_rx_iface = comms;
	}
	osMutexRelease(m_rx_mutex);

	osThreadFlagsSet(m_devp_thread_id, DEVP_FLAGS_RX);
}


static void msg_send_done (comms_layer_t * comms, comms_msg_t * msg, comms_error_t result, void * user)
{
    logger(result == COMMS_SUCCESS ? LOG_DEBUG1: LOG_WARN1, "snt %u", result);
    osThreadFlagsSet(m_devp_thread_id, DEVP_FLAGS_SENT);
}


static comms_msg_t * errorSeqnum(comms_layer_t * comms, const comms_address_t * destination,
                                 bool exists, int error, uint8_t seqnum)
{
	comms_msg_t * p_msg = comms_pool_get(mp_pool, 0);
	if (NULL != p_msg)
	{
		comms_init_message(comms, p_msg);
		comms_set_packet_type(comms, p_msg, AMID_DEVICE_PARAMETERS);
		comms_set_destination(comms, p_msg, destination);

		dp_error_parameter_seqnum_t* ep =
		    (dp_error_parameter_seqnum_t*)comms_get_payload(comms, p_msg, sizeof(dp_error_parameter_seqnum_t));
		if (NULL != ep)
		{
			ep->header = DP_ERROR_PARAMETER_SEQNUM;
			ep->exists = exists;
			ep->error = error;
			ep->seqnum = seqnum;

			comms_set_payload_length(comms, p_msg, sizeof(dp_error_parameter_seqnum_t));

			return p_msg;
		}

		comms_pool_put(mp_pool, p_msg);
	}
	else warn1("pool");

	return NULL;
}


static comms_msg_t * errorId (comms_layer_t * comms, const comms_address_t * destination,
                              bool exists, int error, const char * idstr, uint8_t idlen)
{
	comms_msg_t * p_msg = comms_pool_get(mp_pool, 0);
	if (NULL != p_msg)
	{
		comms_init_message(comms, p_msg);
		comms_set_packet_type(comms, p_msg, AMID_DEVICE_PARAMETERS);
		comms_set_destination(comms, p_msg, destination);

		dp_error_parameter_id_t* ep =
		    (dp_error_parameter_id_t*)comms_get_payload(comms, p_msg, sizeof(dp_error_parameter_id_t) + idlen);
		if (NULL != ep)
		{
			ep->header = DP_ERROR_PARAMETER_ID;
			ep->exists = exists;
			ep->error = error;
			ep->idlength = idlen;
			memcpy(ep->id, idstr, idlen);

			comms_set_payload_length(comms, p_msg, sizeof(dp_error_parameter_id_t) + idlen);

			return p_msg;
		}

		comms_pool_put(mp_pool, p_msg);
	}
	else warn1("pool");

	return NULL;
}


static comms_msg_t * sendValue(comms_layer_t * comms, const comms_address_t * destination,
                               const char* fid, uint8_t idx, uint8_t tp, void* value, uint8_t length)
{
	debugb3("dp.v[%u] %s", value, length, idx, fid);
	comms_msg_t * p_msg = comms_pool_get(mp_pool, 0);
	if (NULL != p_msg)
	{
		comms_init_message(comms, p_msg);
		comms_set_packet_type(comms, p_msg, AMID_DEVICE_PARAMETERS);
		comms_set_destination(comms, p_msg, destination);

		uint8_t idlen = strlen(fid);
		dp_parameter_t* df =
		    (dp_parameter_t*) comms_get_payload(comms, p_msg, sizeof(dp_parameter_t) + idlen + length);
		if (NULL != df)
		{
			df->header = DP_PARAMETER;
			df->type = tp;
			df->seqnum = idx;
			df->idlength = idlen;
			df->valuelength = length;
			memcpy((uint8_t*)df->id, fid, idlen);
			if(length > 0)
			{
				void * pv = (void*)(((uint8_t*)(df->id)) + idlen); // Pointer to value location
				switch(tp)
				{
					case DP_TYPE_UINT16:
					case DP_TYPE_INT16:
						memcpy_swap(pv, value, sizeof(uint16_t));
					break;

					case DP_TYPE_UINT32:
					case DP_TYPE_INT32:
						memcpy_swap(pv, value, sizeof(uint32_t));
					break;

					case DP_TYPE_UINT64:
					case DP_TYPE_INT64:
						memcpy_swap(pv, value, sizeof(uint64_t));
					break;

					case DP_ARRAY_UINT16:
					case DP_ARRAY_INT16:
						for(int i=0;i<length/sizeof(uint16_t);i++)
						{
							memcpy_swap(((uint16_t*)pv)+i, ((uint16_t*)value)+i, sizeof(uint16_t));
						}
					break;

					case DP_ARRAY_UINT32:
					case DP_ARRAY_INT32:
						for(int i=0;i<length/sizeof(uint32_t);i++)
						{
							memcpy_swap(((uint32_t*)pv)+i, ((uint32_t*)value)+i, sizeof(uint32_t));
						}
					break;

					case DP_ARRAY_UINT64:
					case DP_ARRAY_INT64:
						for(int i=0;i<length/sizeof(uint64_t);i++)
						{
							memcpy_swap(((uint64_t*)pv)+i, ((uint64_t*)value)+i, sizeof(uint64_t));
						}
					break;

					default:
						memcpy(pv, value, length);
					break;
				}
			}

			comms_set_payload_length(comms, p_msg, sizeof(dp_parameter_t) + idlen + length);

			return p_msg;
		}

		comms_pool_put(mp_pool, p_msg);
	}
	else warn1("pool");

	return NULL;
}


// Perform network-to-host transformation for select numeric types
static int setValue(const char * name, DeviceParameterTypes_t tp, void * value, uint8_t size)
{
	switch(tp)
	{
		case DP_TYPE_UINT16:
		case DP_TYPE_INT16:
		{
			if (sizeof(uint16_t) == size)
			{
				uint16_t v;
				memcpy_swap(&v, value, sizeof(uint16_t));
				return devp_set(name, tp, &v, sizeof(uint16_t));
			}
			return DEVP_ESIZE;
		}
		break;

		case DP_TYPE_UINT32:
		case DP_TYPE_INT32:
		{
			if (sizeof(uint32_t) == size)
			{
				uint32_t v;
				memcpy_swap(&v, value, sizeof(uint32_t));
				return devp_set(name, tp, &v, sizeof(uint32_t));
			}
			return DEVP_ESIZE;
		}
		break;

		case DP_TYPE_UINT64:
		case DP_TYPE_INT64:
		{
			if (sizeof(uint64_t) == size)
			{
				uint64_t v;
				memcpy_swap(&v, value, sizeof(uint64_t));
				return devp_set(name, tp, &v, sizeof(uint64_t));
			}
			return DEVP_ESIZE;
		}
		break;

		case DP_ARRAY_UINT16:
		case DP_ARRAY_INT16:
		{
			if (0 == size % sizeof(uint16_t))
			{
				for(int i=0;i<size/sizeof(uint16_t);i++)
				{
					uint16_t v;
					memcpy_swap(&v, ((uint16_t*)value)+i, sizeof(uint16_t));
					memcpy(((uint16_t*)value)+i, &v, sizeof(uint16_t));
				}
				return devp_set(name, tp, value, size);
			}
			return DEVP_ESIZE;
		}
		break;

		case DP_ARRAY_UINT32:
		case DP_ARRAY_INT32:
		{
			if (0 == size % sizeof(uint32_t))
			{
				for(int i=0;i<size/sizeof(uint32_t);i++)
				{
					uint32_t v;
					memcpy_swap(&v, ((uint32_t*)value)+i, sizeof(uint32_t));
					memcpy(((uint32_t*)value)+i, &v, sizeof(uint32_t));
				}
				return devp_set(name, tp, value, size);
			}
			return DEVP_ESIZE;
		}
		break;

		case DP_ARRAY_UINT64:
		case DP_ARRAY_INT64:
		{
			if (0 == size % sizeof(uint64_t))
			{
				for(int i=0;i<size/sizeof(uint64_t);i++)
				{
					uint64_t v;
					memcpy_swap(&v, ((uint64_t*)value)+i, sizeof(uint64_t));
					memcpy(((uint64_t*)value)+i, &v, sizeof(uint64_t));
				}
				return devp_set(name, tp, value, size);
			}
			return DEVP_ESIZE;
		}
		break;

		default:
		{
			return devp_set(name, tp, value, size);
		}
		break;
	}
	return 0;
}


static comms_msg_t * handle_rx (comms_layer_t * p_comms, comms_msg_t * p_rx_msg)
{
	comms_address_t client;
	uint8_t len = comms_get_payload_length(p_comms, p_rx_msg);
	uint8_t * payload = (uint8_t*)comms_get_payload(p_comms, p_rx_msg, len);

	comms_get_source(p_comms, p_rx_msg, &client);

	debugb1("rcv[%p]", payload, len, p_comms);
	if ((len > 0)&&(NULL != payload))
	{
		switch(((uint8_t*)payload)[0])
		{
			case DP_HEARTBEAT:
				// something?
				// Maybe make the info about last heartbeat available for other apps?
			break;

			case DP_GET_PARAMETER_WITH_SEQNUM:
				if(len >= sizeof(dp_get_parameter_seqnum_t))
				{
					dp_get_parameter_seqnum_t* gf = (dp_get_parameter_seqnum_t*)payload;

					const char * name;
					DeviceParameterTypes_t tp;
					uint8_t value[DEVP_MAX_VALUE_SIZE];

					int size = devp_discover_idx(gf->seqnum, &name, &tp, value, sizeof(value));
					if(0 <= size)
					{
						return sendValue(p_comms, &client, name, gf->seqnum, tp, value, size);
					}
					else
					{
						return errorSeqnum(p_comms, &client, DEVP_UNKNOWN != size, -DEVP_UNKNOWN, gf->seqnum);
					}
				}
				else // Bad packet size
				{
					errb1("len", payload, len);
					return errorId(p_comms, &client, false, -DEVP_UNKNOWN, "BAD REQ LEN", 11);
				}
			break;

			case DP_GET_PARAMETER_WITH_ID:
				if(len >= sizeof(dp_get_parameter_id_t))
				{
					dp_get_parameter_id_t* gf = (dp_get_parameter_id_t*)payload;

					char name[24+1];
					if (gf->idlength < sizeof(name))
					{
						uint8_t idx;
						DeviceParameterTypes_t tp;
						uint8_t value[DEVP_MAX_VALUE_SIZE];

						memcpy(name, gf->id, gf->idlength);
						name[gf->idlength] = '\0';

						int size = devp_discover_name(name, &idx, &tp, (void*)value, sizeof(value));
						if(0 <= size)
						{
							return sendValue(p_comms, &client, name, idx, tp, value, size);
						}
						else // No such parameter found
						{
							return errorId(p_comms, &client, DEVP_UNKNOWN != size, -size, name, gf->idlength);
						}
					}
					else // No such parameter found
					{
						err1("id len %d", (int)gf->idlength);
						return errorId(p_comms, &client, false, -DEVP_UNKNOWN, "BAD ID LEN", 10);
					}
				}
				else // Bad packet size
				{
					errb1("len", payload, len);
					return errorId(p_comms, &client, false, -DEVP_UNKNOWN, "BAD REQ LEN", 11);
				}
			break;

			case DP_SET_PARAMETER_WITH_ID:
				if(len >= sizeof(dp_set_parameter_id_t))
				{
					dp_set_parameter_id_t* sf = (dp_set_parameter_id_t*)payload;
					if(len == sizeof(dp_set_parameter_id_t) + sf->idlength + sf->valuelength)
					{
						char name[24+1];

						if (sf->idlength < sizeof(name))
						{
							uint8_t idx;
							DeviceParameterTypes_t tp;
							uint8_t value[DEVP_MAX_VALUE_SIZE];

							memcpy(name, sf->id, sf->idlength);
							name[sf->idlength] = '\0';

							int size = devp_discover_name(name, &idx, &tp, (void*)value, sizeof(value));
							if(0 <= size)
							{
								uint8_t * pnv = ((uint8_t*)(sf->id)) + sf->idlength;
								if(0 <= setValue(name, tp, pnv, sf->valuelength))
								{
									// SUCCESS, get new actual value
									size = devp_discover_name(name, &idx, &tp, (void*)value, sizeof(value));
									if(0 <= size)
									{
										return sendValue(p_comms, &client, name, idx, tp, value, size);
									}
									else // Rather unexpected to fail here, but possible
									{
										warnb1("s %u", value, sf->valuelength, idx);
										return errorId(p_comms, &client, true, -size, name, sf->idlength);
									}
								}
								else
								{
									warnb1("s %u", value, sf->valuelength, idx);
									return errorId(p_comms, &client, true, -size, name, sf->idlength);
								}
							}
							else
							{
								warnb1("s %u", value, sf->valuelength, idx);
								return errorId(p_comms, &client, DEVP_UNKNOWN != size, -size, name, sf->idlength);
							}
						}
						else // Bad id len
						{
							err1("id len %d", (int)sf->idlength);
							return errorId(p_comms, &client, false, -DEVP_UNKNOWN, "BAD ID LEN", 10);
						}
					}
					else // Bad packet size
					{
						errb1("len", payload, len);
						return errorId(p_comms, &client, false, -DEVP_UNKNOWN, "BAD REQ LEN", 11);
					}
				}
			break;
			default:
				warnb1("dflt", payload, len);
			break;
		}
	}
	return NULL;
}


static void sleep_block_comms (comms_layer_t * p_comms)
{
	while(osOK != osMutexAcquire(m_rcvr_mutex, osWaitForever));
	for (int i=0;i<DEVP_MAX_IFACES;i++)
	{
		if(p_comms == mp_ifaces[i])
		{
			if(NULL != mp_sleep_ctrl[i]) // Sleep control is optional
			{
				#ifdef SERVICE_MODE_TIMEOUT
				m_rcomm_keepalive_timeout = devp_service_mode_timeout_get();
				#endif //SERVICE_MODE_TIMEOUT
				if(!m_rcomm_keepalive_timeout)
				{
					m_rcomm_keepalive_timeout = DEVP_DEFAULT_SLEEP_TIMEOUT_MS;
				}
				debug1("block %d for %d sec", i, m_rcomm_keepalive_timeout/1000);
				comms_sleep_block(mp_sleep_ctrl[i]);
				osTimerStart(m_sleep_timer, m_rcomm_keepalive_timeout);
			}
			break;
		}
	}
	osMutexRelease(m_rcvr_mutex);
}


static void sleep_allow_comms (void)
{
	while(osOK != osMutexAcquire(m_rcvr_mutex, osWaitForever));
	for(int i=0;i<DEVP_MAX_IFACES;i++)
	{
		if (NULL != mp_sleep_ctrl[i])
		{
			debug1("sleep %d", i);
			comms_sleep_allow(mp_sleep_ctrl[i]);
		}
	}
	osMutexRelease(m_rcvr_mutex);
}


static void devp_comms_loop (void * arg)
{
	comms_msg_t * p_msg = NULL;
	for(;;)
	{
		uint32_t flags = osThreadFlagsWait(DEVP_FLAGS_ALL, osFlagsWaitAny, osWaitForever);

		if (flags & DEVP_FLAGS_SENT)
		{
			comms_pool_put(mp_pool, p_msg);
			p_msg = NULL;
		}

		if (NULL == p_msg)
		{
			comms_layer_t * p_comms = NULL;
			while(osOK != osMutexAcquire(m_rx_mutex, osWaitForever));
			if (NULL != mp_rx_iface)
			{
				p_comms = mp_rx_iface;
				p_msg = handle_rx(mp_rx_iface, &m_rx_msg);
				mp_rx_iface = NULL;
			}
			osMutexRelease(m_rx_mutex);

			if (NULL != p_msg) // Something is happening, keep the interface awake for a bit
			{
				sleep_block_comms(p_comms);

				comms_error_t err = comms_send(p_comms, p_msg, msg_send_done, NULL);
				logger(COMMS_SUCCESS == err ? LOG_DEBUG1: LOG_WARN1, "snd %u", err);
				if (err != COMMS_SUCCESS)
				{
					comms_pool_put(mp_pool, p_msg);
					p_msg = NULL;
				}
			}
		}

		if (flags & DEVP_FLAGS_SLEEP) // All interfaces put to sleep together
		{
			sleep_allow_comms();
		}
	}
}

static void sleep_timer_fired_cb()
{
	osThreadFlagsSet(m_devp_thread_id, DEVP_FLAGS_SLEEP);
}


bool devp_comms_init (comms_pool_t * p_pool)
{
	mp_pool = p_pool;
	m_rcomm_keepalive_timeout = DEVP_DEFAULT_SLEEP_TIMEOUT_MS;
	const osMutexAttr_t mutex_param = { .attr_bits = osMutexPrioInherit };

	m_rx_mutex = osMutexNew(&mutex_param);
	if (NULL == m_rx_mutex)
	{
		return false;
	}

	m_rcvr_mutex = osMutexNew(&mutex_param);
	if (NULL == m_rcvr_mutex)
	{
		osMutexDelete(m_rx_mutex);
		return false;
	}

	m_sleep_timer = osTimerNew(sleep_timer_fired_cb, osTimerOnce, NULL, NULL);
	if (NULL == m_sleep_timer)
	{
		osMutexDelete(m_rcvr_mutex);
		osMutexDelete(m_rx_mutex);
		return false;
	}

	for(int i=0;i<DEVP_MAX_IFACES;i++)
	{
		mp_ifaces[i] = NULL;
	}

    // Create a thread
    const osThreadAttr_t thread_attr = { .name = "devp", .stack_size = 1920 };
    m_devp_thread_id = osThreadNew(devp_comms_loop, NULL, &thread_attr);

    if (NULL == m_devp_thread_id)
    {
    	osTimerDelete(m_sleep_timer);
		osMutexDelete(m_rcvr_mutex);
		osMutexDelete(m_rx_mutex);
    	return false;
    }
    return true;
}


int devp_add_iface(comms_layer_t * comms, comms_sleep_controller_t * ctrl, bool heartbeat)
{
	int ret = -1;
	while(osOK != osMutexAcquire(m_rcvr_mutex, osWaitForever));
	for(int i=0;i<DEVP_MAX_IFACES;i++)
	{
		if(mp_ifaces[i] == comms)
		{
			ret = -2;
			break;
		}
		else if(NULL == mp_ifaces[i])
		{
			mp_ifaces[i] = comms;
			mp_sleep_ctrl[i] = ctrl;
			m_heartbeat[i] = heartbeat;

			if(NULL != ctrl)
			{
				comms_register_sleep_controller(comms, ctrl, radio_status_changed, NULL);
			}

			comms_register_recv(comms, &(m_rcvrs[i]), receive_message, NULL, AMID_DEVICE_PARAMETERS);
			ret = i;
			break;
		}
	}
	osMutexRelease(m_rcvr_mutex);
	return ret;
}


int devp_remove_iface(comms_layer_t * comms)
{
	int ret = -1;
	while(osOK != osMutexAcquire(m_rcvr_mutex, osWaitForever));
	for(int i=0;i<DEVP_MAX_IFACES;i++)
	{
		if(mp_ifaces[i] == comms)
		{
			comms_deregister_recv(comms, &(m_rcvrs[i]));

			if (NULL != mp_sleep_ctrl[i])
			{
				comms_deregister_sleep_controller(comms, mp_sleep_ctrl[i]);
				mp_sleep_ctrl[i] = NULL;
			}

			mp_ifaces[i] = NULL;
			ret = i;
			break;
		}
	}
	osMutexRelease(m_rcvr_mutex);
	return ret;
}
