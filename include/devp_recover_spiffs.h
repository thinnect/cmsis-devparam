#ifndef DEVP_RECOVER_SPIFFS_H_
#define DEVP_RECOVER_SPIFFS_H_

#include "fs.h"

bool devp_recover_spiffs (fs_driver_t * driver, int partition);
bool devp_recover_migrate (void);
bool devp_recover_check (void);

#endif//DEVP_RECOVER_SPIFFS_H_
