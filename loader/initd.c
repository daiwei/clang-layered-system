#include <sys/shm.h>
#include <common/sys.h>
#include <common/log.h>
#include <common/str.h>
#include <common/def.h>

#define all_item(_do)                       \
    _do ## _str(sys, log_path);             \
                                            \
    _do ## _int(workmode, mode);            \
    _do ## _int(workmode, interval);        \
    _do ## _int(workmode, interval_accoff); \
    _do ## _int(workmode, distance);        \
                                            \
    _do ## _str(device, imei);              \
    _do ## _int(device, type);              \
    _do ## _str(device, version);           \
    _do ## _str(device, update_url);        \
    _do ## _int(device, ping_interval);     \
    _do ## _str(device, sos);               \
    _do ## _str(device, sos1);              \
    _do ## _str(device, sos2);              \
    _do ## _str(device, sos3);              \
    _do ## _int(device, timezone);          \
    _do ## _str(device, sim_no);            \
                                            \
    _do ## _str(gsm, apn);                  \
    _do ## _str(gsm, apnuser);              \
    _do ## _str(gsm, apnpasswd);            \
                                            \
    _do ## _str(server, host);              \
    _do ## _int(server, port);              \
    _do ## _str(server, host1);             \
    _do ## _int(server, port1);


static int shmid = -1;
static env_t *_env = NULL;
static FILE *inifile = NULL;
static dictionary *inidict = NULL;

static int parse_conf()
{
    inifile = fopen(SYS_CONF_PATH, "r+");
    inidict = iniparser_load(SYS_CONF_PATH);
    if (!inifile || !inidict) {
        log_debug("can't load '%s'", SYS_CONF_PATH);
        exit(EXIT_FAILURE);
    }

    #define parse_int(_s, _n)           _env->_s._n = iniparser_getint(inidict, #_s ":" #_n, 0)
    #define parse_bool(_s, _n)          _env->_s._n = iniparser_getboolean(inidict, #_s ":" #_n, -1)
    #define parse_str(_s, _n)           strcpy(_env->_s._n, iniparser_getstring(inidict, #_s ":" #_n, NULL))
    #define parse_double(_s, _n)        _env->_s._n = iniparser_getdouble(inidict, #_s ":" #_n, 0.0)
    all_item(parse);

    return 1;
}

static int update_conf(dictionary *inidict)
{
    if (!inifile || !inidict) {
        log_debug("can't load '%s'", SYS_CONF_PATH);
        exit(EXIT_FAILURE);
    }

    #define update(_s, _n, _f) \
        do { \
            char _buf[1024] = {0}; \
            snprintf(_buf, 1024, _f, _env->_s._n); \
            iniparser_set(inidict, #_s ":" #_n, _buf); \
        } while (0)

    #define update_int(_s, _n)          update(_s, _n, "%d")
    #define update_bool(_s, _n)         update(_s, _n, "%d")
    #define update_str(_s, _n)          update(_s, _n, "%s")
    #define update_double(_s, _n)       update(_s, _n, "%f")
    all_item(update);

    iniparser_dump_ini(inidict, inifile);
    iniparser_freedict(inidict);
    fclose(inifile);

    return 1;
}

void init_sys_env()
{
    log_debug("env size: %d", sizeof(env_t));

    shmid = shmget(SYS_ENV_SHM, sizeof(env_t), 0);
    if (shmid != -1) {
        shmctl(shmid, IPC_RMID, 0);
    }

    log_debug("create sys env shm.");
    shmid = shmget(SYS_ENV_SHM, sizeof(env_t), IPC_CREAT | 0660);
    if (shmid == -1) {
        log_debug("shmget failed. err: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    _env = (env_t *)shmat(shmid, 0, 0);
    if (_env == (void*)-1) {
        log_debug("shmat failed. err: %s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    memset(_env, 0, sizeof(env_t));
    log_debug("shm attached at %p", _env);

    log_debug("init sys info from config.");
    parse_conf();

    log_debug("%s", _env->device.imei);
    log_debug("%s", _env->server.host);
    log_debug("%d", _env->server.port);
}

void sigroutine(int sig)
{
    // _env->server.port = 4700;
    // strcpy(_env->device.imei, "025001");
    log_debug("update inidict file");
    update_conf(inidict);

    log_debug("remove env shm");
    shmctl(shmid, IPC_RMID, 0);

    log_debug("env()->pids is %d", env()->pids);
    for (int i = 0; i < env()->pids; i++) {
        pid_t pid = env()->pidv[i];
        log_debug("kill %ld", pid);
        if (pid <= 0) break;
        kill(pid, SIGTERM);
    }

    exit(0);
}

int main(int argc, char** argv)
{
    // *** change current working directory *************************
    char current_path[1024];
    realpath(dirname(argv[0]), current_path);

    int index = str_substr(str_new__cstr(current_path), "init");
    str_t gateway_path = str_new__cstr2(current_path, index - 1);

    chdir(gateway_path);
    setenv("GATEWAY_PATH", gateway_path, 1);
    // **************************************************************

    init_sys_env();
    log_init("initd");

    // signal(SIGINT,  sigroutine);
    // signal(SIGQUIT, sigroutine);
    signal(SIGTERM, sigroutine);

    log_debug("initd loader sleep...");
    while(1) {
        sleep(INT_MAX);
    }
}
