#ifndef __FRAMEWORK_POWER_H__
#define __FRAMEWORK_POWER_H__

#include <common/module.h>

typedef struct {
    int (*workmode)();
    int (*update_device)();
    int (*sync_device)();
    int (*recover_device)();
    int (*reboot_device)();
    int (*shutdown_device)();
} power_interface_t;

#endif//__FRAMEWORK_POWER_H__
