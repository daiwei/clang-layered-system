/*
 * ================================
 * Variation of DBUS RPC Parameters
 * ================================
 *
 *  |<--------------------------------------- rpc-cli ----------------------------------------->|
 *  arg => rpc_arg => byte_array .......... call & block ........... byte_array => rpc_arg => arg
 *                     /                                                      \
 *                   /                                                          \
 *                 /                                                              \
 *           byte_array => rpc_arg => arg => func(arg...) => arg => rpc_arg => byte_array
 *            + malloc                        + malloc for out arg
 *           |<------------------------------ rpc-srv --------------------------------->|
 *
 */

#ifndef __COMMON_RPC_HELPER_H__
#define __COMMON_RPC_HELPER_H__

#include <dbus/dbus.h>
#include <common/util.h>

/**
 * reg rpc function, max args size is 5
 */
#define RPC_METHOD(_b, _f, _s, ...)         _RPC_METHOD_##_s(_b, _f, ##__VA_ARGS__)

#define _RPC_METHOD_0(_b, _f) \
    int _f(); \
    int _rpc_cli_##_f() \
    { \
        log_debug("enter function %s", __FUNCTION__); \
        RPC_CLI_BEGIN(0); \
        RPC_CLI_CALL(_b, _f); \
        RPC_CLI_END(); \
    } \
    \
    int _rpc_srv_##_f(byte **pbuf, int *psize) \
    { \
        log_debug("enter function %s", __FUNCTION__); \
        RPC_SRV_BEGIN(0); \
        _f(); \
        RPC_SRV_END(); \
    }


#define _RPC_METHOD_1(_b, _f, _1t, _1s) \
    int _f(_1t _1); \
    int _rpc_cli_##_f(_1t _1) \
    { \
        log_debug("enter function %s", __FUNCTION__); \
        RPC_CLI_BEGIN(1); \
        RPC_CLI_ARG_SET(_1, _1s); \
        RPC_CLI_CALL(_b, _f); \
        RPC_CLI_END(); \
    } \
    \
    int _rpc_srv_##_f(byte **pbuf, int *psize) \
    { \
        log_debug("enter function %s", __FUNCTION__); \
        RPC_SRV_BEGIN(1); \
        _f(_args[0]->buf); \
        RPC_SRV_END(); \
    }


#define _RPC_METHOD_2(_b, _f, _1t, _1s, _2t, _2s) \
    int _f(_1t _1, _2t _2); \
    int _rpc_cli_##_f(_1t _1, _2t _2) \
    { \
        log_debug("enter function %s", __FUNCTION__); \
        RPC_CLI_BEGIN(2); \
        RPC_CLI_ARG_SET(_1, _1s); \
        RPC_CLI_ARG_SET(_2, _2s); \
        RPC_CLI_CALL(_b, _f); \
        RPC_CLI_END(); \
    } \
    \
    int _rpc_srv_##_f(byte **pbuf, int *psize) \
    { \
        log_debug("enter function %s", __FUNCTION__); \
        RPC_SRV_BEGIN(2); \
        _f(_args[0]->buf, _args[1]->buf); \
        RPC_SRV_END(); \
    }


#define _RPC_METHOD_3(_b, _f, _1t, _1s, _2t, _2s, _3t, _3s) \
    int _f(_1t _1, _2t _2, _3t _3); \
    int _rpc_cli_##_f(_1t _1, _2t _2, _3t _3) \
    { \
        log_debug("enter function %s", __FUNCTION__); \
        RPC_CLI_BEGIN(3); \
        RPC_CLI_ARG_SET(_1, _1s); \
        RPC_CLI_ARG_SET(_2, _2s); \
        RPC_CLI_ARG_SET(_3, _3s); \
        RPC_CLI_CALL(_b, _f); \
        RPC_CLI_END(); \
    } \
    \
    int _rpc_srv_##_f(byte **pbuf, int *psize) \
    { \
        log_debug("enter function %s", __FUNCTION__); \
        RPC_SRV_BEGIN(3); \
        _f(_args[0]->buf, _args[1]->buf, _args[2]->buf); \
        RPC_SRV_END(); \
    }


#define _RPC_METHOD_4(_b, _f, _1t, _1s, _2t, _2s, _3t, _3s, _4t, _4s) \
    int _f(_1t _1, _2t _2, _3t _3, _4t _4); \
    int _rpc_cli_##_f(_1t _1, _2t _2, _3t _3, _4t _4) \
    { \
        log_debug("enter function %s", __FUNCTION__); \
        RPC_CLI_BEGIN(4); \
        RPC_CLI_ARG_SET(_1, _1s); \
        RPC_CLI_ARG_SET(_2, _2s); \
        RPC_CLI_ARG_SET(_3, _3s); \
        RPC_CLI_ARG_SET(_4, _4s); \
        RPC_CLI_CALL(_b, _f); \
        RPC_CLI_END(); \
    } \
    \
    int _rpc_srv_##_f(byte **pbuf, int *psize) \
    { \
        log_debug("enter function %s", __FUNCTION__); \
        RPC_SRV_BEGIN(4); \
        _f(_args[0]->buf, _args[1]->buf, _args[2]->buf, _args[3]->buf); \
        RPC_SRV_END(); \
    }


#define _RPC_METHOD_5(_b, _f, _1t, _1s, _2t, _2s, _3t, _3s, _4t, _4s, _5t, _5s) \
    int _f(_1t _1, _2t _2, _3t _3, _4t _4, _5t _5); \
    int _rpc_cli_##_f(_1t _1, _2t _2, _3t _3, _4t _4, _5t _5) \
    { \
        log_debug("enter function %s", __FUNCTION__); \
        RPC_CLI_BEGIN(5); \
        RPC_CLI_ARG_SET(_1, _1s); \
        RPC_CLI_ARG_SET(_2, _2s); \
        RPC_CLI_ARG_SET(_3, _3s); \
        RPC_CLI_ARG_SET(_4, _4s); \
        RPC_CLI_ARG_SET(_5, _5s); \
        RPC_CLI_CALL(_b, _f); \
        RPC_CLI_END(); \
    } \
    \
    int _rpc_srv_##_f(byte **pbuf, int *psize) \
    { \
        log_debug("enter function %s", __FUNCTION__); \
        RPC_SRV_BEGIN(5); \
        _f(_args[0]->buf, _args[1]->buf, _args[2]->buf, _args[3]->buf, _args[4]->buf); \
        RPC_SRV_END(); \
    }


/**
 * rpc cli & srv function define
 *
 * buf introduce:
 * rpc_arg_t* _args[_n]
 *
 * _args[0]->buf => _args + 0
 *         |
 *         V
 *         pplocation
 *
 * _args + 0 => realloc
 *
 * plocation => _args[0]->buf
 */
////////////////////////////////////////////////////////////////////////////////////////////////////
// client side
////////////////////////////////////////////////////////////////////////////////////////////////////
#define RPC_CLI_BEGIN(_n) \
    byte * _buf = NULL; \
    long _len = 0; \
    rpc_arg_t _args[_n]; \
    int _args_size = _n; \
    int _arg_idx = 0;


#define RPC_CLI_ARG_SET(_a, _s) \
    if (!_a) { \
        log_debug("error: arg in is null, exit rpc."); \
        return 0; \
    } \
    _args[_arg_idx].len = _s; \
    _args[_arg_idx].ref = (void *)_a; \
    _len += (_s + RPC_ARG_SIZE); \
    log_debug("arg %d len= %d, ref= %p", _arg_idx, _args[_arg_idx].len, _args[_arg_idx].ref); \
    _arg_idx++; \

    // arg_packs[arg_pack_n].size = _s; \
    // arg_packs[arg_pack_n].buf = (void *)_a; \
    // /*log_debug("arg %d size= %d, buf= %p", arg_pack_n, (int)_s, _a);*/ \
    // _len += (arg_packs[arg_pack_n].size + sizeof(rpc_arg_t)); \
    // arg_pack_n++;


#define RPC_CLI_CALL(_b, _m) \
    log_debug("calloc _len: %ld", _len); \
    /*_buf = calloc(1, _len); /*TODO*/ \
    _buf = dbus_malloc0(_len); /*TODO*/ \
    int _offset = 0; \
    for (int i = 0; i < _args_size; i++) { \
        rpc_arg_t *arg = (rpc_arg_t *)(_buf + _offset); \
        arg->len = _args[i].len; \
        arg->ref = _args[i].ref; \
        memcpy(arg->buf, arg->ref, arg->len); \
        _offset += (_args[i].len + RPC_ARG_SIZE); \
    } \
    int ret = msg_method_call(_b, INFO.rpc_name, INFO.rpc_iface, #_m, &_buf, &_len); \
    if (_b == NO_REPLY) return ret; \
    log_debug("--------------------------- return len: %d", _len);


#define RPC_CLI_END() \
    byte * _buf_offset = _buf; \
    for (int i = 0; i < _args_size; i++) { \
        rpc_arg_t *arg = (rpc_arg_t*)_buf_offset; \
        if (_args[i].len == OUT_SIZE) { \
            void **addr = _args[i].ref; \
            *addr = calloc(1, arg->len); /*FREE BY USER*/\
            log_debug("================ malloc *addr %p", *addr); \
            memcpy(*addr, arg->buf, arg->len); \
        } else if (_args[i].len == arg->len) { \
            memcpy(_args[i].ref, arg->buf, arg->len); \
        } else { \
            log_debug("_args[%d].len = %d, arg->len = %d", i, _args[i].len, arg->len); \
            log_error("result len wrong!"); \
        } \
        _buf_offset += (arg->len + RPC_ARG_SIZE); \
    } \
    log_debug("RPC_CLI_END exit"); \
    return ret;


////////////////////////////////////////////////////////////////////////////////////////////////////
// server side
////////////////////////////////////////////////////////////////////////////////////////////////////
#define RPC_SRV_BEGIN(_n) \
    /*log_debug("arg len= %d, buf= %p", (int)len, buf);*/ \
    rpc_arg_t* _args[_n]; \
    int _args_size = _n; \
    long _offset = 0; \
    for (int i = 0; i < _args_size; i++) { \
        _args[i] = (rpc_arg_t*)(*pbuf + _offset); \
        if (_args[i]->len == OUT_SIZE) { \
            _args[i]->ref = &_args[i]; \
        } \
        _offset += (_args[i]->len + RPC_ARG_SIZE); \
    }

#define RPC_SRV_END() \
    /*dbus_free(*pbuf);*/ \
    int total_len = 0; \
    for (int i = 0; i < _args_size; i++) { \
        total_len += (_args[i]->len + RPC_ARG_SIZE); \
    } \
    *psize = total_len; \
    /**pbuf = calloc(1, total_len); /*TODO*/ \
    *pbuf = dbus_malloc0(total_len); /*TODO*/ \
    log_debug("--------------------------- buf addr %p", *pbuf); \
    _offset = 0; \
    for (int i = 0; i < _args_size; i++) { \
        log_debug("--------------------------- _args[%d]->len: %d", i, _args[i]->len); \
        memcpy(*pbuf + _offset, _args[i], _args[i]->len + RPC_ARG_SIZE); \
        _offset += (_args[i]->len + RPC_ARG_SIZE); \
    } \
    log_debug("RPC_SRV_END exit"); \
    return 0;


void print_iter(DBusMessageIter *iter, dbus_bool_t literal, int depth);
void print_rpc_msg(DBusMessage *message, dbus_bool_t literal);

#endif  // __COMMON_RPC_HELPER_H__
