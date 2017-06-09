#include <common/def.h>
#include <common/module.h>
#include <common/list.h>
#include <common/notify.h>
#include <common/str.h>
#include <common/util.h>

/* processor single */
static rpc_mgr_t *g_rpc_mgr;

static int rpc_msg_iter_append_byte_array(DBusMessageIter *iter, const unsigned char *value, unsigned int len)
{
    DBusMessageIter iter_array;
    unsigned int i;

    if (!dbus_message_iter_open_container(iter, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE_AS_STRING, &iter_array))
        return FAILURE;

    dbus_message_iter_append_fixed_array(&iter_array, DBUS_TYPE_BYTE, &value, len);
    // for (i = 0; i < len; i++) {
    //     if (!dbus_message_iter_append_basic(&iter_array, DBUS_TYPE_BYTE, &(value[i])))
    //         return FAILURE;
    // }

    if (!dbus_message_iter_close_container(iter, &iter_array))
        return FAILURE;

    return SUCCESS;
}


static DBusHandlerResult rpc_method_filter(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
    log_debug("enter function %s", __FUNCTION__);

    // log_debug("msg destination: %s", dbus_message_get_destination(msg));
    // log_debug("msg path:        %s", dbus_message_get_path(msg));
    // log_debug("msg iface:       %s", dbus_message_get_interface(msg));
    // log_debug("msg member:      %s", dbus_message_get_member(msg));

    DBusMessage *reply = NULL;
    dbus_uint32_t serial = 0;

    // log_debug("module_size: %d", g_rpc_mgr->module_size);
    for (int i = 0; i < g_rpc_mgr->module_size; i++) {
        module_t *module = g_rpc_mgr->modules[g_rpc_mgr->module_size - 1];
        // log_debug("method_regs size: %d", list_size(module->method_regs));
        list_iter_t *iter = list_iter(module->method_regs);
        while (list_iter_has_next(iter)) {
            list_node_t *node = list_iter_next(iter);
            method_reg_t *mr = node->data;
            if (!mr) {
                log_debug("handler is null");
                break;
            }

            if (dbus_message_is_method_call(msg, module->info->rpc_iface, mr->name)) {
                log_debug("run: %s", mr->name);
                reply = msg_method_invoke(msg, NULL, mr->handler);
                break;
            }
        }
        list_iter_free(iter);
    }

    if (reply != NULL) {
        if (!dbus_connection_send(conn, reply, &serial)) {
            log_debug("out of memory!");
            exit(1);
        }
        dbus_connection_flush(conn);
    }

    return DBUS_HANDLER_RESULT_HANDLED;
}

// TODO: !!only loader can call those
static DBusHandlerResult rpc_command_filter(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
    log_debug("enter function %s", __FUNCTION__);
    for (int i = 0; i < g_rpc_mgr->module_size; i++) {
        module_t *module = g_rpc_mgr->modules[g_rpc_mgr->module_size - 1];

        if (dbus_message_is_method_call(msg, module->info->rpc_iface, MSG_CMD_START)) {
            // module->start(module);
        } else if (dbus_message_is_method_call(msg, module->info->rpc_iface, MSG_CMD_PAUSE)) {
            // module->pause(module);
        } else if (dbus_message_is_method_call(msg, module->info->rpc_iface, MSG_CMD_RESTART)) {
            // module->restart(module);
        } else if (dbus_message_is_method_call(msg, module->info->rpc_iface, MSG_CMD_STOP)) {
            // module->stop(module);
        } else {

        }
    }

    return DBUS_HANDLER_RESULT_HANDLED;
}

static DBusHandlerResult rpc_notify_filter(DBusConnection *conn, DBusMessage *msg, void *user_data)
{
    log_debug("enter function %s", __FUNCTION__);
    if (dbus_message_is_signal(msg, RPC_NOTIFY, MSG_NOTIFY_SHOCK)) {
        list_t *handlers = notify_get_handlers(MSG_NOTIFY_SHOCK);
        list_iter_t *iter = list_iter(handlers);
        while (list_iter_has_next(iter)) {
            list_node_t *node = list_iter_next(iter);
            notify_handler_t handler = node->data;
            handler();
        }
        list_iter_free(iter);
    } else if (dbus_message_is_signal(msg, RPC_NOTIFY, MSG_NOTIFY_REBOOT)) {
        //TODO
    } else if (dbus_message_is_signal(msg, RPC_NOTIFY, MSG_NOTIFY_SHUTDOWN)) {
        //TODO
    }

    return DBUS_HANDLER_RESULT_HANDLED;
}

void rpc_init(char *name)
{
    log_debug("enter function %s", __FUNCTION__);

    // if there is no env DISPLAY, dbus will report error
    char *env_display = getenv("DISPLAY");
    if (env_display == NULL || strlen(env_display) == 0) {
        log_debug("env $DISPLAY is null, set $DISPLAY");
        setenv("DISPLAY", ":0", true);
    }
    // ****************************************************

    // ***NECESSARY*** DBus in multiple threads
    dbus_threads_init_default();

    DBusError err;
    dbus_error_init(&err);

    g_rpc_mgr = calloc(1, sizeof(rpc_mgr_t));
    g_rpc_mgr->name = name;

    // ***NECESSARY*** DBus in multiple threads
    g_rpc_mgr->cli_conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);
    if (dbus_error_is_set(&err)) {
        log_debug("connection error: %s", err.message);
        dbus_error_free(&err);
    }
    if (NULL == g_rpc_mgr->cli_conn) {
        log_debug("connection is null");
        exit(1);
    }
}

void rpc_reg(module_t *module)
{
    log_debug("enter function %s", __FUNCTION__);
    if (!module) {
        log_debug("module is null");
        return;
    }

    log_debug("reg module: %s", module->info->module_id);
    g_rpc_mgr->modules[g_rpc_mgr->module_size] = module;
    g_rpc_mgr->module_size++;
}

static void *_rpc_listen(void *arg)
{
    // ***NECESSARY*** DBus in multiple threads
    dbus_threads_init_default();

    DBusError err;
    dbus_error_init(&err);

    // ***NECESSARY*** DBus in multiple threads
    g_rpc_mgr->srv_conn = dbus_bus_get_private(DBUS_BUS_SESSION, &err);

    // request our name on the bus and check for errors
    log_debug("dbus name: %s", g_rpc_mgr->name);
    // RPC_NAME -> Processor ID
    int ret = dbus_bus_request_name(g_rpc_mgr->srv_conn, g_rpc_mgr->name, DBUS_NAME_FLAG_REPLACE_EXISTING, &err);
    if (dbus_error_is_set(&err)) {
        log_debug("name error: %s", err.message);
        dbus_error_free(&err);
    }
    if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) {
        log_debug("not primary owner (%d)", ret);
        exit(1);
    }

    // if (g_rpc_mgr->method_size > 0) {
    // log_debug("add rpc_method_filter");
    dbus_connection_add_filter(g_rpc_mgr->srv_conn, rpc_method_filter, NULL, NULL);
    // }
    // if (g_rpc_mgr->module) {
    // log_debug("add rpc_command_filter & rpc_notify_filter");
    dbus_connection_add_filter(g_rpc_mgr->srv_conn, rpc_command_filter, NULL, NULL);
    dbus_connection_add_filter(g_rpc_mgr->srv_conn, rpc_notify_filter, NULL, NULL);
    // }

    while (dbus_connection_read_write_dispatch(g_rpc_mgr->srv_conn, -1)) ;  // loop

    return NULL;
}

void rpc_listen()
{
    log_debug("enter function %s", __FUNCTION__);
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, _rpc_listen, NULL);
    if (ret != 0) {
        log_debug("create pthread error!");
    }
}

interface_t *get_module_depend(module_t *module, const char *layer_id, const char *module_id)
{
    // log_debug("enter function %s", __FUNCTION__);
    if (module == NULL) {
        log_debug("module == NULL");
        return NULL;
    }

    if (!module->depends) {
        return NULL;
    }

    char key[MAX_DESC] = {0};
    sprintf(key, "%s.%s", layer_id, module_id);
    log_debug("get depend %s", key);

    interface_t *iface = hash_get(module->depends, key);

    return iface;
}

int reg_module_depend(module_t *module, const char *layer_id, const char *module_id, interface_t *dep)
{
    log_debug("enter function %s", __FUNCTION__);
    if (module == NULL) {
        log_debug("module == NULL");
        return 0;
    }

    if (!module->depends) {
        module->depends = hash_new();
    }

    char key[MAX_DESC] = {0};
    sprintf(key, "%s.%s", layer_id, module_id);
    log_debug("reg depend %s", key);

    hash_set(module->depends, key, dep);

    log_debug("leave function %s", __FUNCTION__);
    return 0;
}

module_t *get_module(char *layer_id, char *module_id)
{
    log_debug("enter function %s", __FUNCTION__);
    str_t layer  = str_new__cstr(layer_id);
    str_t module = str_new__cstr(module_id);

    for (int i = 0; i < g_rpc_mgr->module_size; i++) {
        module_t *module = g_rpc_mgr->modules[g_rpc_mgr->module_size - 1];
        if (str_equal(layer, module->info->layer_id)
            && str_equal(module, module->info->module_id))
        {
            return module;
        }
    }

    return NULL;
}

int msg_method_call(bool block, char *name, char *iface, char *member, byte **pbuf, int *psize)
{
    log_debug("enter function %s", __FUNCTION__);
    if (!pbuf || !psize) {
        log_error("pbuf || psize is null.");
    }

    DBusMessage *request, *reply;
    DBusError err;
    DBusMessageIter iter;
    DBusMessageIter subiter;

    int ret = SUCCESS;

     log_debug("dbus message call name  : %s", name);
     log_debug("dbus message call path  : %s", PRC_PATH);
     log_debug("dbus message call iface : %s", iface);
     log_debug("dbus message call member: %s", member);
    request = dbus_message_new_method_call(name, PRC_PATH, iface, member);

    dbus_error_init(&err);
    if (*pbuf) {
        dbus_message_iter_init_append(request, &iter);
        rpc_msg_iter_append_byte_array(&iter, *pbuf, *psize);
    }

    if (block == NO_REPLY) {
        log_debug("send dbus message #unblock");
        dbus_connection_send(g_rpc_mgr->cli_conn, request, NULL);
        dbus_message_unref(request);
    } else {
        log_debug("send dbus message #block");
        reply = dbus_connection_send_with_reply_and_block(g_rpc_mgr->cli_conn, request, -1, &err);
        dbus_message_unref(request);

        if (NULL == reply) {
            if (dbus_error_is_set(&err)) {
                log_error("an error occurred: %s", err.message);
                dbus_error_free(&err);
            }
            log_debug("NULL == reply");
            return FAILURE;
        }

        //if (*pbuf) free(*pbuf);
        if (*pbuf) dbus_free(*pbuf);

        dbus_message_iter_init(reply, &iter);
        dbus_message_iter_recurse(&iter, &subiter);
        dbus_message_iter_get_fixed_array(&subiter, pbuf, psize); /* *pbuf should not be freed */

        dbus_message_unref(reply);
    }

    return ret;
}

DBusMessage *msg_method_invoke(DBusMessage *message, void *user_data, rpc_handler handler)
{
    log_debug("enter function %s", __FUNCTION__);
    if (!message || !handler) {
        log_debug("error message: %p, handler: %p", message, handler);
        return NULL;
    }

    DBusMessage *reply = NULL;
    DBusError err;
    DBusMessageIter iter;

    byte *buf = NULL;
    int size = 0;
    int ret = SUCCESS;

    dbus_error_init(&err);
    if (!(dbus_message_get_args(message, &err, DBUS_TYPE_ARRAY, DBUS_TYPE_BYTE, &buf, &size, DBUS_TYPE_INVALID))) {
        log_debug("unable to get input args");
        if (dbus_error_is_set(&err)) {
            log_debug("%s raised: %s", err.name, err.message);
        }
        buf = NULL;
        size = 0;
    }
    dbus_error_free(&err);

    ret = handler(&buf, &size);
    reply = dbus_message_new_method_return(message);
    if (buf) {
        dbus_message_iter_init_append(reply, &iter);
        rpc_msg_iter_append_byte_array(&iter, buf, size);
    } else {
        log_debug("buf is null");
    }

    dbus_free(buf);

    return reply;
}

int rpc_send_signal(char *notify, byte *buf, int size)
{
    log_debug("enter function %s", __FUNCTION__);
    if (!buf) {
        log_debug("buf is null");
        return 0;
    }

    DBusMessage *message;
    DBusError err;
    DBusMessageIter iter;
    DBusMessageIter subiter;

    int ret = SUCCESS;

    log_debug("dbus message call path  : %s", PRC_PATH);
    log_debug("dbus message call iface : %s", RPC_NOTIFY);
    log_debug("dbus message call notify: %s", notify);
    message = dbus_message_new_signal(PRC_PATH, RPC_NOTIFY, notify);

    dbus_message_iter_init_append(message, &iter);
    rpc_msg_iter_append_byte_array(&iter, buf, size);

    log_debug("send dbus signal");
    dbus_connection_send(g_rpc_mgr->cli_conn, message, NULL);

    dbus_message_unref(message);

    return ret;
}

int msg_command(module_t *module, char *cmd)
{
    log_debug("enter function %s", __FUNCTION__);
    if (!module) return 0;
    return msg_method_call(WAIT_REPLY, module->info->rpc_name, module->info->rpc_iface, cmd, NULL, NULL);
}

/**********************************************************
* notify
**********************************************************/
int msg_notify(char *notify)
{
    log_debug("enter function %s", __FUNCTION__);
    return rpc_send_signal(notify, NULL, 0);
}

int reg_notify(char *notify, notify_handler_t handler)
{
    log_debug("enter function %s", __FUNCTION__);
    notify_add_handler(notify, handler);
    return 0;
}

void rpc_arg_malloc(void **pbuf, long size)
{
    byte *pb = pbuf;
    rpc_arg_t *arg = (rpc_arg_t*)(pb - RPC_ARG_SIZE);
    if (arg->len == OUT_SIZE) {
        rpc_arg_t** parg = (rpc_arg_t**)(arg->ref);
        *parg = calloc(1, size + RPC_ARG_SIZE);
        // *parg = dbus_malloc0(size + RPC_ARG_SIZE);
        log_debug("rpc_arg_malloc ==> %p", *parg);
        (*parg)->len = size;
        *pbuf = (*parg)->buf;
    }
}
