/**
 * Migrating deviceparameters storage file from SPIFFS based FS to LittleFS based FS.
 */
#include "devp_recover_spiffs.h"

#include "spiffs.h"
#include "devp_storage.h"
#include "spi_flash.h"
#include "checksum.h"

#include "loglevels.h"
#define __MODUUL__ "rdevp"
#define __LOG_LEVEL__ (LOG_LEVEL_devp_recover_spiffs & BASE_LOG_LEVEL)
#include "log.h"

#define DEVP_SPIFFS_LOG_PAGE_SZ  (128UL)
#define DEVP_SPIFFS_LOG_BLOCK_SZ (32UL * 1024UL)

#define DEVP_BACKUP_STORAGE_OFFSET 4096

static spiffs_config m_cfg;
static spiffs m_fs;
static uint8_t m_work_buf[DEVP_SPIFFS_LOG_PAGE_SZ * 2];
static uint8_t m_fds[44]; // SPIFFS_buffer_bytes_for_filedescs
static fs_driver_t * mp_driver;
static int m_partition;

struct devp_backup_header
{
    uint32_t length; // 12 + sizeof(data)
    char d;
    char e;
    char v;
    char p;
    uint32_t version;
    uint8_t data[];
} __attribute((packed));

static int32_t fs_read0 (uint32_t addr, uint32_t size, uint8_t * dst)
{
	if (mp_driver->read(m_partition, addr, size, dst) < 0)
	{
		return SPIFFS_ERR_INTERNAL;
	}
	return SPIFFS_OK;
}

bool devp_recover_spiffs (fs_driver_t * driver, int partition)
{
	mp_driver = driver;
    m_partition = partition;

    m_cfg.phys_size = driver->size(partition);
	m_cfg.phys_addr = 0;
	m_cfg.phys_erase_block = driver->erase_size(partition);
	m_cfg.log_block_size = DEVP_SPIFFS_LOG_BLOCK_SZ;
	m_cfg.log_page_size = DEVP_SPIFFS_LOG_PAGE_SZ;
    m_cfg.hal_read_f = fs_read0;
    m_cfg.hal_write_f = NULL;
    m_cfg.hal_erase_f = NULL;

    driver->lock();

	int ret = SPIFFS_mount(&m_fs, &m_cfg, m_work_buf, m_fds, sizeof(m_fds), NULL, 0, NULL);
    if(SPIFFS_OK != ret)
    {
        debug1("ret %d", ret);
        driver->unlock();
        return false;
    }

    spiffs_stat stat;
    int32_t statres = SPIFFS_stat(&m_fs, DEVP_STORAGE_FNAME, &stat);
    if (statres != 0)
    {
        debug1("nostat %d", statres);
        SPIFFS_unmount(&m_fs);
        driver->unlock();
        return false;
    }

    if (stat.size <= 0)
    {
        debug1("devp1 s %d", stat.size);
        SPIFFS_unmount(&m_fs);
        driver->unlock();
        return false;
    }

    spiffs_file sfd = SPIFFS_open(&m_fs, DEVP_STORAGE_FNAME, SPIFFS_RDONLY, 0);
    if (sfd < 0)
    {
        debug1("open %d", sfd);
        driver->unlock();
        return false;
    }
    driver->unlock();

    // Now we copy it elsewhere
    spi_flash_lock();
    spi_flash_erase(0, DEVP_BACKUP_STORAGE_OFFSET, spi_flash_erase_size(0));
    spi_flash_unlock();

    struct devp_backup_header hdr = {sizeof(struct devp_backup_header) + stat.size, 'd', 'e', 'v', 'p', 1};
    uint32_t crc = crc_32((const unsigned char*)&hdr, sizeof(hdr));

    int offset = 0;
    do
    {
        uint8_t data;
        driver->lock();
        ret = SPIFFS_read(&m_fs, sfd, &data, 1);
        driver->unlock();
        if (ret > 0)
        {
            warn1("RD %02X\n", data);
            crc = update_crc_32(crc, data);
            spi_flash_lock();
            spi_flash_write(0, DEVP_BACKUP_STORAGE_OFFSET + sizeof(struct devp_backup_header) + offset, 1, &data);
            spi_flash_unlock();
            offset++;
        }
    }
    while (ret > 0);

    bool result = false;
    if (offset > 0)
    {
        debug1("len %d hlen %d crc %x", stat.size, hdr.length, crc);
        if (offset != stat.size)
        {
            debug1("off size %d != %d", offset, stat.size);
        }

        spi_flash_lock();
        spi_flash_write(0, DEVP_BACKUP_STORAGE_OFFSET, sizeof(hdr), (uint8_t*)&hdr);
        spi_flash_write(0, DEVP_BACKUP_STORAGE_OFFSET + sizeof(struct devp_backup_header) + offset, sizeof(crc), (uint8_t*)&crc);
        spi_flash_unlock();
        result = true;
    }
    else
    {
        debug1("nodata");
    }

    driver->lock();
    SPIFFS_close(&m_fs, sfd);
    SPIFFS_unmount(&m_fs);
    driver->unlock();

    return result;
}

bool devp_recover_migrate (void)
{
    struct devp_backup_header hdr;
    spi_flash_lock();
    spi_flash_read(0, DEVP_BACKUP_STORAGE_OFFSET, sizeof(hdr), (uint8_t*)&hdr);
    spi_flash_unlock();
    if ((hdr.d == 'd')
      &&(hdr.e == 'e')
      &&(hdr.v == 'v')
      &&(hdr.p == 'p')
      &&(hdr.version == 1))
    {
        debug1("backup found %d", hdr.length);
    }
    else
    {
        return false;
    }

    uint32_t datalen = hdr.length - sizeof(struct devp_backup_header);
    uint32_t crc = crc_32((const unsigned char*)&hdr, sizeof(hdr));
    spi_flash_lock();
    for (int offs = sizeof(hdr); offs < sizeof(hdr) + datalen; offs++)
    {
        uint8_t data;
        spi_flash_read(0, DEVP_BACKUP_STORAGE_OFFSET + offs, 1, &data);
        warn1("RD %02X\n", data);
        crc = update_crc_32(crc, data);
    }
    uint32_t scrc;
    spi_flash_read(0, DEVP_BACKUP_STORAGE_OFFSET + hdr.length, 4, (uint8_t*)&scrc);
    spi_flash_unlock();
    if (crc == scrc)
    {
        debug1("crc ok");
    }
    else
    {
        debug1("crc err %x != %x", crc, scrc);
        return false;
    }

    // Copy snapshot from temporary raw area storage to FS
    fs_fd fd = fs_open(DEVP_STORAGE_FS_ID, DEVP_STORAGE_FNAME, SPIFFS_CREAT | SPIFFS_EXCL);
    if (fd < 0)
    {
        return false;
    }

    for (int offs = sizeof(hdr); offs < sizeof(hdr) + datalen; offs++)
    {
        uint8_t data;
        spi_flash_lock();
        spi_flash_read(0, DEVP_BACKUP_STORAGE_OFFSET + sizeof(hdr) + offs, 1, &data);
        spi_flash_unlock();

        int32_t wr = fs_write(DEVP_STORAGE_FS_ID, fd, &data, 1);
        if (wr < 1)
        {
            debug1("wr %d", wr);
            return false;
        }
    }

    fs_close(DEVP_STORAGE_FS_ID, fd);

    // TODO Should we erase it?

    return true;
}

bool devp_recover_check (void)
{
    fs_fd fd = fs_open(DEVP_STORAGE_FS_ID, DEVP_STORAGE_FNAME, SPIFFS_RDONLY);
    if (fd < 0)
    {
        return false;
    }
    fs_close(DEVP_STORAGE_FS_ID, fd);
    return true;
}
