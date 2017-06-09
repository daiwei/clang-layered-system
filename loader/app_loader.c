#include <common/module.h>
#include <common/loader.h>
#include <common/str.h>
#include <common/def.h>

int launch_app(const char *id)
{
    log_debug("enter function %s", __FUNCTION__);

    app_t *app = (app_t*)load(ID_APP, id);

    module_t      *base = GET_MODULE(app);
    module_info_t *info = GET_MODULE_INFO(app);

    pid_t pid = fork();
    if (pid == 0) {     /* child processor */
        log_debug("child  processor(%d)|app %s start", getpid(), info->module_id);

        //TODO: change user

        /* rpc */
        rpc_init(info->rpc_name);
        rpc_reg(base);
        rpc_listen(); //run as a thread, so can be called in case main process stopped

        app->start();
        log_debug("app start end");
    } else if(pid > 0) {/* parent processor */
        log_debug("parent processor(%d)", getpid());
        reg_pid(pid);
        return 1;
    } else {
        log_debug("app %s start error", info->module_id);
        return 0;
    }

    return 0;
}

int kill_app(const char *id)
{

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
    log_init("app");

    // launch_app("demo");
    dictionary *inidict = iniparser_load(LOADER_CONF_PATH);
    if (!inidict) {
        log_debug("can't load '%s'", LOADER_CONF_PATH);
        exit(EXIT_FAILURE);
    }

    char buf[1024];
    strcpy(buf, iniparser_getstring(inidict, "loader:app", NULL));
    str_t sbuf = str_new__cstr(buf);
    str_t *modules = str_split(sbuf, ",");
    str_t *p = modules;
    while (*p) {
        log_debug("module %s", *p);
        *p = str_trim(*p);
        launch_app(*p);
        str_release(*p);
        p++;
    }
    free(modules);

    log_debug("app loader sleep...");
    while(1) {
        // sleep(INT_MAX);
        pid_t epid = waitpid(-1, NULL, 0);
        if (epid > 0) {
            log_debug("app %d exit", epid);
        } else {
            sleep(1);
        }
    }
    return 1;
}
