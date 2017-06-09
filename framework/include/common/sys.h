/**
 * system only
 */
#ifndef __FRAMEWORK_SYS_H__
#define __FRAMEWORK_SYS_H__

#include <common/def.h>
#include <common/util.h>
#include <sys/types.h>
#include <unistd.h>

#define SYS_CONF_PATH           "conf/sys.conf"
#define SYS_ENV_SHM             0x83321137


#define WM_FIX_TIME             0
#define WM_TRIGGING             1
#define WM_FIX_DISTANCE         2
#define WM_ACCOFF_FIX_TIME      3
#define WM_ACCOFF_TRIGGING      4
#define WM_PASSIVE              5

typedef struct {
    char            log_path[MAX_URL];
} sysconf_t;

typedef struct {
    int             mode;
    int             interval;
    int             interval_accoff;
    int             distance;
} workmode_t;

typedef struct {
    char            imei[MAX_ID];
    int             type;
    char            version[MAX_VERSION];
    int             reg_time;

    bool            is_acc_on;
    int             voltage;

    char            update_url[MAX_URL];
    int             ping_interval;
    bool            fuel_circuit_on;
    char            sos[MAX_PHONE];
    char            sos1[MAX_PHONE];
    char            sos2[MAX_PHONE];
    char            sos3[MAX_PHONE];
    float           timezone;
    char            sim_no[MAX_PHONE];

    int             cnt_fuel;
    int             cnt_enginer;
    int             cnt_mileage;
} device_info_t;

typedef struct {
    char            apn[MAX_NAME];
    char            apnuser[MAX_NAME];
    char            apnpasswd[MAX_PASS];
} gsm_info_t;

typedef struct {
    char            host[MAX_IPV4];
    int             port;
    char            host1[MAX_IPV4];
    int             port1;

    bool            need_auth;
    char            user[MAX_NAME];
    char            passwd[MAX_PASS];

    int             srv_fd;

    bool            is_connect;
    bool            is_login;
} server_info_t;

typedef struct {
    bool            is_inited;
    sysconf_t       sys;
    workmode_t      workmode;
    device_info_t   device;
    gsm_info_t      gsm;
    server_info_t   server;
    pid_t           pidv[100];
    int             pids;
} env_t;

env_t *env();
void reg_pid(pid_t pid);

#endif//__FRAMEWORK_SYS_H__
