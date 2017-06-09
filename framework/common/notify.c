#include <common/notify.h>
#include <common/log.h>

static list_t *g_notify_list = NULL;

list_t *notify_list()
{
    if (!g_notify_list) {
        g_notify_list = list_new();
    }
    return g_notify_list;
}

int log_notify(void *data)
{
    notify_info_t *nl = data;
    log_debug("%s", nl->notify);
}

static void _add_notify(char *notify)
{
    log_debug("enter function %s", __FUNCTION__);
    notify_info_t *nl = calloc(1, sizeof(notify_info_t));
    nl->notify = notify;
    nl->handlers = list_new();

    list_rpush(notify_list(), nl);
    list_foreach(notify_list(), log_notify);
}

int log_handler(void *data)
{
    log_debug("%p", data);
}

static void _add_handler(list_t *handlers, notify_handler_t handler)
{
    log_debug("enter function %s", __FUNCTION__);
    if (!handlers) {
        log_debug("arg is null");
        return;
    }

    list_rpush(handlers, handler);
    list_foreach(handlers, log_handler);
}

static int notify_compare(void *data1, void *data2)
{
    notify_info_t *nl1 = data1;
    notify_info_t *nl2 = data2;
    log_debug("nl1: %s; nl2: %s", nl1->notify, nl2->notify);
    return strcmp(nl1->notify, nl2->notify);
}

void notify_add_handler(char *notify, notify_handler_t handler)
{
    log_debug("add handler(%p) for notify(%s)", handler, notify);
    notify_info_t tmp = { .notify = notify };

    list_node_t *node = list_get__data(notify_list(), &tmp, notify_compare);
    if (!node) {
        _add_notify(notify);
        node = list_get__data(notify_list(), &tmp, notify_compare);
    }

    _add_handler(((notify_info_t *)node->data)->handlers, handler);
}

list_t *notify_get_handlers(char *notify)
{
    notify_info_t tmp = { .notify = notify };
    list_node_t *node = list_get__data(notify_list(), &tmp, notify_compare);
    return ((notify_info_t *)node->data)->handlers;
}
