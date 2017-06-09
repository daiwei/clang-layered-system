#ifndef __COMMON_MODULE_H__
#define __COMMON_MODULE_H__

#include <dbus/dbus.h>
#include <common/def.h>
#include <common/util.h>
#include <common/sys.h>
#include <common/rpc_helper.h>
#include <common/list.h>
#include <common/hash.h>
#include <common/log.h>

/**********************************************************
 * macro define
 **********************************************************/
#define ID_HARDWARE                     "hardware"
#define ID_FRAMEWORK                    "framework"
#define ID_SERVICE                      "service"
#define ID_APP                          "app"

#define PRC_PATH                        "/com/embedded/gateway"
#define RPC_NAME_PRE                    "com.embedded.gateway."
#define RPC_IFACE_PRE                   "com.embedded.gateway."
#define RPC_NOTIFY                      "com.embedded.gateway.notify"

#define INFO                            _info_
#define INFO_STR                        "_info_"

#define MODULE                          _module_
#define MODULE_STR                      "_module_"

#define INTERFACE                       _interface_
#define INTERFACE_STR                   "_interface_"
#define RPC_FRAMEWORK_NAME              "com.embedded.gateway.framework"

#ifndef RPC_METHOD
#   define RPC_METHOD(_b, _f, _s, ...)  _RPC_METHOD_##_s(_b, _f, ##__VA_ARGS__)
#endif

#define GET_MODULE(_m)                  ((module_t *)_m)
#define GET_MODULE_INFO(_m)             (_m)->info
// #define GET_MODULE_INFO(_m)             GET_MODULE(_m)->info

typedef void (*dylib_init_t)();
#define DYLIB_INIT                      _dylib_init_
#define DYLIB_INIT_STR                  "_dylib_init_"

#define OUT_SIZE                        0
// #define BLOCK                           1
// #define UNBLOCK                         0
#define WAIT_REPLY                      1
#define NO_REPLY                        0

// #define EXPORT(_iface_t) \
//     _iface_t  INTERFACE; \
//     void _dylib_init_()
#define EXPORT(_iface_t, _block) \
    _iface_t  INTERFACE; \
    void _dylib_init_() { \
        log_debug("_dylib_init_"); \
        GET_MODULE(this)->method_regs = list_new(); \
        _block; \
    }

#define RPC_REG(_m) \
    do { \
        INTERFACE._m = _rpc_cli_##_m; \
        method_reg_t _mr = { #_m, _rpc_srv_##_m }; \
        list_lpush__clone(GET_MODULE(this)->method_regs, &_mr, sizeof(method_reg_t)); \
    } while (0)


#define MODULE_FRAMEWORK(_id) \
    static int on_load(); \
    static int on_unload(); \
    static module_info_t INFO = { \
        .layer_id   = "framework", \
        .module_id  = _id, \
        .rpc_name   = RPC_FRAMEWORK_NAME, \
        .rpc_iface  = "module." _id, \
        .on_load    = on_load, \
        .on_unload  = on_unload \
    }; \
    framework_t MODULE = { \
        .size = sizeof(framework_t), \
        .info = &INFO \
    }; \
    static framework_t *this = &MODULE


#define MODULE_SERVICE(_id) \
    static int on_load(); \
    static int on_unload(); \
    static module_info_t INFO = { \
        .layer_id   = "service", \
        .module_id  = _id, \
        .rpc_name   = RPC_NAME_PRE "service." _id, \
        .rpc_iface  = "module." _id, \
        .on_load    = on_load, \
        .on_unload  = on_unload \
    }; \
    service_t MODULE = { \
        .size = sizeof(service_t), \
        .info = &INFO \
    }; \
    static service_t *this = &MODULE

#define MODULE_APP(_id) \
    static int on_load(); \
    static int on_unload(); \
    static module_info_t INFO = { \
        .layer_id   = "app", \
        .module_id  = _id, \
        .rpc_name   = RPC_NAME_PRE "app." _id, \
        .rpc_iface  = "module." _id, \
        .on_load    = on_load, \
        .on_unload  = on_unload \
    }; \
    app_t MODULE = { \
        .size = sizeof(app_t), \
        .info = &INFO \
    }; \
    static app_t *this = &MODULE


#define IMPORT_FRAMEWORK(_id)   (void *)import(GET_MODULE(this), "framework", _id)
#define IMPORT_SERVICE(_id)     (void *)import(GET_MODULE(this), "service",   _id)
#define IMPORT_APP(_id)         (void *)import(GET_MODULE(this), "app",       _id)


/**********************************************************
 * type define
 **********************************************************/
typedef int (*rpc_handler)(byte **, int *);

typedef enum { STOP, INIT, RUNNING, DESTROY } module_status_t;

// typedef struct {
//     long  size;
//     void *buf;
// } arg_pack_t;

typedef struct {
    long len;
    void *ref;
    byte buf[0];
} rpc_arg_t;
#define RPC_ARG_SIZE            sizeof(rpc_arg_t)

typedef struct {
    char        name[MAX_NAME];
    rpc_handler handler;
} method_reg_t;

typedef struct {
    char layer_id[MAX_ID];
    char module_id[MAX_ID];
    char rpc_name[MAX_DESC];
    char rpc_iface[MAX_DESC];

    int (*on_load  )();
    int (*on_unload)();
} module_info_t;

/**
 * HERITABLE* ->open api
 *
 * function type in interface_t:
 *   int (*iface)(void *, ...);
 */
typedef struct {
    /* int (*iface)(void *, ...); */
} interface_t;

// *HERITABLE* -> so info
typedef struct {
#define MODULE_BASE                 \
    long            size;           \
    module_info_t   *info;          \
    module_status_t status;         \
    list_t          *method_regs;   \
    hash_t          *depends;       \
    void            *dlhandle;

    MODULE_BASE;
} module_t;

typedef struct {
    MODULE_BASE;

    int (*init   )();
    int (*start  )();
    int (*pause  )();
    int (*restart)();
    int (*stop   )();
    int (*destroy)();
} app_t;

typedef struct {
    MODULE_BASE;

    int (*init   )();
    int (*start  )();
    int (*pause  )();
    int (*restart)();
    int (*stop   )();
    int (*destroy)();
} service_t;

typedef struct {
    MODULE_BASE;
    //...
} framework_t;

typedef struct {
    char            *name;
    DBusConnection  *cli_conn;
    DBusConnection  *srv_conn;
    int              module_size;
    module_t        *modules[SYS_MAX_MODULE];
} rpc_mgr_t;

/**********************************************************
 * function declare
 **********************************************************/
// rpc process method
void rpc_init(char *name);
void rpc_reg(module_t *module);

void rpc_listen();

interface_t *get_module_depend(module_t *module, const char *layer_id, const char *module_id);
int reg_module_depend(module_t *module, const char *layer_id, const char *module_id, interface_t *dep);

module_t *get_module(char *layer_id, char *module_id);

// #define get_depend(_s)      get_module_depend(&MODULE, _s)
// #define reg_depend(_i)      reg_module_depend(&MODULE, _i)


DBusMessage *msg_method_invoke(DBusMessage *message, void *user_data, rpc_handler handler);
int msg_method_call(bool block, char *name, char *iface, char *member, byte **buf, int *size);

int msg_command(module_t *module, char *cmd);

int msg_notify(char *notify);

void rpc_arg_malloc(void **buf, long size);

#undef MODULE_BASE
#endif //__COMMON_MODULE_H__
