#include <common/module.h>
#include <common/loader.h>
#include <common/str.h>
#include <common/def.h>

int launch_service(const char *id)
{
    log_debug("enter function %s", __FUNCTION__);

    service_t *service = (service_t*)load(ID_SERVICE, id);

    module_t      *base = GET_MODULE(service);
    module_info_t *info = GET_MODULE_INFO(service);

    pid_t pid = fork();
    if (pid == 0) {     /* child processor */
        log_debug("child  processor(%d)|service %s start", getpid(), info->module_id);

        /* rpc */
        rpc_init(info->rpc_name);
        rpc_reg(base);
        rpc_listen(); //run as a thread, so can be called in case main process stopped

        service->start();
        log_debug("service start end");
    } else if (pid > 0) {/* parent processor */
        log_debug("parent processor(%d)", getpid());
        reg_pid(pid);
        return 1;
    } else {
        log_debug("service %s start error", info->module_id);
        return 0;
    }

    return 0;
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

    reg_pid(getpid());
    log_init("service");

    // launch_service("location");
    // launch_service("tracker");
    // // launch_service("lighttpd");
    dictionary *inidict = iniparser_load(LOADER_CONF_PATH);
    if (!inidict) {
        log_debug("can't load '%s'", LOADER_CONF_PATH);
        exit(EXIT_FAILURE);
    }

    char buf[1024];
    strcpy(buf, iniparser_getstring(inidict, "loader:service", NULL));
    str_t sbuf = str_new__cstr(buf);
    str_t *modules = str_split(sbuf, ",");
    str_t *p = modules;
    while (*p) {
        log_debug("module %s", *p);
        *p = str_trim(*p);
        launch_service(*p);
        str_release(*p);
        p++;
    }
    free(modules);

    log_debug("service loader sleep...");
    while(1) {
        // sleep(INT_MAX);
        pid_t epid = waitpid(-1, NULL, 0);
        if (epid > 0) {
            log_debug("service %d exit", epid);
        } else {
            sleep(1);
        }
    }
    return 1;
}
