#ifndef __SERVICE_INSTALLER_H__
#define __SERVICE_INSTALLER_H__

#include <common/module.h>


#define PKG_PATH        "download"
#define UNZIP_PATH      "tmp"
#define APP_PATH        "app"

#define PKG_SYS         1
#define PKG_APP         2


typedef struct {
    char md5sum[32];
    char sem0;
    char gmtime[19];
    char sem1;
    char pkgtype[7];
} app_mark_t;


#endif//__SERVICE_INSTALLER_H__
