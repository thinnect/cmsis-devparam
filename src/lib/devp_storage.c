/**
 * DeviceParameters storage implementation.
 *
 * Copyright Thinnect Inc. 2020
 * @author Raido Pahtma
 * @license MIT
 **/
#include "devp_storage.h"
#include "devp.h"

#include "fs.h"

#include "loglevels.h"
#define __MODUUL__ "dps"
#define __LOG_LEVEL__ (LOG_LEVEL_devp_storage & BASE_LOG_LEVEL)
#include "log.h"

#define DEVP_STORAGE_FNAME "devp1"

bool devp_storage_init(devp_storage_t * strg)
{
	strg->f = (int32_t)fs_open(DEVP_STORAGE_FNAME, SPIFFS_CREAT | SPIFFS_RDWR);
	if (strg->f < 0)
	{
		warn1("fail");
		return false;
	}
	debug1("f=%d", (int)strg->f);
	return true;
}

#pragma pack(push, 1)
typedef struct devp_storage_element
{
	uint8_t tlen; // Total length, determines length of actual value field, crc can be closer
	char name[17];
	uint8_t vlen;
	uint8_t value[64]; // This is variable, actual size is tlen - (20)
	uint16_t crc;
} devp_storage_element_t;
#pragma pack(pop)

#define DEVPS_RECORD_NAME_LENGTH (16)
#define DEVPS_RECORD_CRC_LENGTH (2)
#define DEVPS_RECORD_MIN_LENGTH (DEVPS_RECORD_NAME_LENGTH+1+1+DEVPS_RECORD_CRC_LENGTH)
#define DEVPS_RECORD_MAX_LENGTH (DEVPS_RECORD_MIN_LENGTH+64)

// return 0 for found, parameters updated
// return POSITIVE for EOF reached, offset contains value
// return NEGATIVE for errors
static int find_parameter(fs_fd f, const char * name,
	uint32_t * offset, uint8_t * length, uint8_t * maxlength,
	void * value, uint8_t size)
{
	uint32_t offs = 0;

	fs_lseek(f, offs, SEEK_SET);

	for (;;)
	{
		devp_storage_element_t record;
		if (sizeof(record.tlen) == fs_read(f, &(record.tlen), sizeof(record.tlen)))
		{
			offs += sizeof(record.tlen);
			if((record.tlen >= DEVPS_RECORD_MIN_LENGTH)&&(record.tlen <= DEVPS_RECORD_MAX_LENGTH))
			{
				if(fs_read(f, record.name, record.tlen) == record.tlen)
				{
					offs += record.tlen;
					// TODO check CRC

					if('\0' == record.name[DEVPS_RECORD_NAME_LENGTH]) // Name is \0 filled to the end, final byte must be a \0
					{
						if(0 == strcmp(record.name, name))
						{
							uint8_t len = record.tlen - DEVPS_RECORD_MIN_LENGTH;
							if(record.vlen > len)
							{
								warn1("vlen");
								return -1;
							}
							if(NULL != value)
							{
								if(len > size)
								{
									warn1("len");
									return -1;
								}
								memcpy(value, record.value, record.vlen);
							}
							if(NULL != offset)
							{
								*offset = offs - sizeof(record.tlen) - record.tlen;
							}
							if(NULL != maxlength)
							{
								*maxlength = len;
							}
							if(NULL != length)
							{
								*length = record.vlen;
							}
							debugb1("%s:", record.value, record.vlen, name);
							return 0;
						}
					}
					else
					{
						warn1("bad name");
						return -1;
					}
				}
				else
				{
					warn1("ld != rlen");
					return -1;
				}
			}
			else
			{
				warn1("rlen %d", (int)record.tlen);
				return -1;
			}
		}
		else
		{
			debug1("EOF @ %d", (int)offs); // Successfully reached end of file without finding parameter
			if(NULL != offset)
			{
				*offset = offs;
			}
			break;
		}
	}

	return 1; // EOF
}

int devp_storage_load(devp_storage_t * strg, const char * name, void * value, uint8_t length)
{
	fs_fd f = (fs_fd)strg->f;
	if (f < 0)
	{
		warn1("nofile");
		return DEVP_UNKNOWN;
	}

	uint8_t len;
	int r = find_parameter(f, name, NULL, &len, NULL, value, length);

	if(0 == r)
	{
		return len;
	}

	return DEVP_UNKNOWN;
}

static bool write_record(fs_fd f, uint32_t offset, const char * name, uint8_t maxlength, const void * value, uint8_t length)
{
	devp_storage_element_t record;
	uint8_t nlen = strlen(name);

	if(nlen > DEVPS_RECORD_NAME_LENGTH)
	{
		return false;
	}
	memcpy(record.name, name, nlen);
	memset(&(record.name[nlen]), '\0', (DEVPS_RECORD_NAME_LENGTH+1)-nlen);

	record.vlen = length;
	memcpy(&(record.value), value, length);
	memset(&(record.value[length]), 0, maxlength-length); // memset rest of buffer

	record.tlen = DEVPS_RECORD_MIN_LENGTH + maxlength;
	uint8_t wlen = 1 + record.tlen;

	// TODO calculate actual CRC
	uint16_t crc = 0xABCD;
	((uint8_t*)&record)[wlen-1] = crc >> 8;
	((uint8_t*)&record)[wlen-2] = crc;

	fs_lseek(f, offset, SEEK_SET);
	debugb1("write @%d", &record, wlen, (int)offset);
	if(wlen != fs_write(f, &record, wlen))
	{
		err1("write");
		return false;
	}

	fs_flush(f);
	return true;
}

int devp_storage_save(devp_storage_t * strg, const char * name, const void * value, uint8_t length, uint8_t maxlength)
{
	fs_fd f = (fs_fd)strg->f;
	if (f < 0)
	{
		warn1("nofile");
		return DEVP_UNKNOWN;
	}

	uint32_t offset;
	uint8_t maxlen;
	int r = find_parameter(f, name, &offset, NULL, &maxlen, NULL, 0);

	if(r < 0)
	{
		warn1("erase"); // Something is broken, start from scratch
		fs_unlink(DEVP_STORAGE_FNAME);
		devp_storage_init(strg);
		offset = 0;
		// TODO must also rewrite any other persistent parameters
	}
	else if(r == 0)
	{
		if(maxlength > maxlen)
		{
			warn1("bad fit"); // Variable does not fit, start from scratch
			fs_unlink(DEVP_STORAGE_FNAME);
			devp_storage_init(strg);
			offset = 0;
			// TODO must also rewrite any other persistent parameters
		}
	}

	if(write_record(f, offset, name, maxlength, value, length))
	{
		return length;
	}

	return DEVP_UNKNOWN;
}
