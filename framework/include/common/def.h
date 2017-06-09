#ifndef __COMMON_DEF_H__
#define __COMMON_DEF_H__

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <limits.h>
#include <stdarg.h>
#include <math.h>
#include <pthread.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <libgen.h>
#include <sys/time.h>
#include <iniparser/iniparser.h>

/**
 * type define
 */
// #define bool                    int
#define false                   0
#define true                    1

#define int16                   int16_t
#define uint16                  uint16_t
#define int32                   int32_t
#define uint32                  uint32_t
#define int64                   int64_t
#define uint64                  uint64_t
#define uint                    unsigned int
#define byte                    char
// #define addr                    char*

/**
 * constant define
 */
#define CPU_BIT_WIDTH           sizeof(long)

#define MAX_VERSION             32
#define MAX_ID                  64
#define MAX_NAME                64
#define MAX_PASS                256
#define MAX_DESC                256
#define MAX_URL                 256
#define MAX_FILE                256

#define MAX_IPV4                16
#define MAX_IPV6                64
#define MAX_PHONE               32

#define MAX_DEPEND              256
#define MAX_METHOD              256

#define SYS_MAX_MODULE          10240
#define SYS_MAX_HANDLER         10240

#define PI                      3.14159265359
#define EARTH_R                 6378137

#define LAYER_APP               "app"
#define LAYER_FRAMEWORK         "framework"


#define MSG_CMD_START           "start"
#define MSG_CMD_PAUSE           "pause"
#define MSG_CMD_RESTART         "restart"
#define MSG_CMD_STOP            "stop"
#define MSG_CMD_DESTROY         "destroy"


#define MSG_NOTIFY_ACC          "acc"
#define MSG_NOTIFY_SHOCK        "shock"
#define MSG_NOTIFY_LOW_BATTERY  "low_battery"
#define MSG_NOTIFY_SOS          "sos"
#define MSG_NOTIFY_WIRE_CUT     "wire_cut"
#define MSG_NOTIFY_SD_FULL      "sd_full"
#define MSG_NOTIFY_SHUTDOWN     "shutdown"
#define MSG_NOTIFY_REBOOT       "reboot"

/**
 * result type define
 */
#define SUCCESS                 0
#define FAILURE                 1


#define LOADER_CONF_PATH        "conf/loader.conf"


#endif/*__COMMON_DEF_H__*/
