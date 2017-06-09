#ifndef __COMMON_NOTIFY_H__
#define __COMMON_NOTIFY_H__

#include <common/list.h>

typedef int (*notify_handler_t)();

typedef struct {
    char *notify;
    list_t *handlers;
} notify_info_t;

list_t *notify_list();
void notify_add_handler(char *notify, notify_handler_t handler);
list_t *notify_get_handlers(char *notify);

#endif//__COMMON_NOTIFY_H__
