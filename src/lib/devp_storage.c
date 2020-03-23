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

bool devp_storage_init(devp_storage_t * strg)
{
	return true;
}

int devp_storage_load(devp_storage_t * strg, const char * name, void * value, uint8_t length)
{
	int ret = DEVP_UNKNOWN;

	fs_fd f = fs_open("devp1", SPIFFS_O_RDONLY);
	if (f < 0)
	{
		warn1("nofile");
		return DEVP_UNKNOWN;
	}


	for (;;)
	{
		uint8_t rlen = 0;
		if (sizeof(rlen) == fs_read(f, &rlen, 1))
		{
			if((rlen >= 18)&&(rlen < 100))
			{
				uint8_t buf[rlen];
				if(fs_read(f, buf, rlen) == rlen)
				{
					// TODO check CRC

					if('\0' == buf[16]) // Name is \0 filled to the end, final byte must be a \0
					{
						if(0 == strcmp((char*)buf, name))
						{
							uint8_t len = rlen - 17 - 2;
							if(len <= length)
							{
								debugb1("%s:", &buf[17], len, name);
								memcpy(value, &buf[17], len);
								ret = len;
							}
							else
							{
								warn1("len");
								ret = DEVP_ESIZE;
							}
							break;
						}
					}
					else
					{
						warn1("bad name");
						break;
					}
				}
				else
				{
					warn1("ld != rlen");
					break;
				}
			}
			else
			{
				warn1("rlen %d", (int)rlen);
				break;
			}
		}
		else
		{
			debug1("EOF");
			break;
		}
	}

	fs_close(f);
	return ret;
}

int devp_storage_save(devp_storage_t * strg, const char * name, const void * value, uint8_t length, uint8_t maxlength)
{
	int ret = DEVP_UNKNOWN;

	fs_fd f = fs_open("devp1", SPIFFS_CREAT | SPIFFS_RDWR);
	if (f < 0)
	{
		warn1("nofile");
		return DEVP_UNKNOWN;
	}

	uint32_t offset = 0;
	for (;;)
	{
		uint8_t rlen = 0;
		if (sizeof(rlen) == fs_read(f, &rlen, 1))
		{
			offset += sizeof(rlen);
			if((rlen >= 19)&&(rlen < 100))
			{
				uint8_t buf[rlen];
				if(fs_read(f, buf, rlen) == rlen)
				{
					offset += rlen;
					// TODO check CRC

					if('\0' == buf[16]) // Name is \0 filled to the end, final byte must be a \0
					{
						if(0 == strcmp((char*)buf, name))
						{
							uint8_t len = rlen - 17 - 2;
							if(len == maxlength)
							{
								offset = offset - (len + 2);
								if(fs_lseek(f, offset, SEEK_SET) == offset)
								{
									// TODO write actual value length first
									fs_write(f, value, length);
									// TODO write CRC also
									debug1("write %s @%d", name, (int)offset);
									ret = length;
								}
							}
							else
							{
								ret = DEVP_ESIZE;
							}
							break;
						}
					}
					else
					{
						warn1("bad name");
						break;
					}
				}
				else
				{
					warn1("ld != rlen");
					break;
				}
			}
			else
			{
				warn1("rlen %d", (int)rlen);
				break;
			}
		}
		else
		{
			debug1("EOF @%d", (int)offset);

			uint8_t len = 17+maxlength+2;
			uint8_t nlen = strlen(name);
			uint8_t buf[1+len];
			buf[0] = len;
			memcpy(&buf[1], name, nlen);
			memset(&buf[1+nlen], '\0', 17-nlen);
			// TODO actual length
			memcpy(&buf[1+17], value, length);
			// memset() rest of buffer
			// TODO CRC
			debugb1("write", buf, 1+len);
			if((1+len) != fs_write(f, buf, 1+len))
			{
				err1("write");
			}
			// TODO write entry here now
			ret = len;
			break;
		}
	}

	fs_close(f);

	return ret;
}