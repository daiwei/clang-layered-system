// #include <framework/location.h>
#include <common/def.h>
#include <common/module.h>
#include <common/loader.h>
#include <common/util.h>
#include <common/str.h>

int launch_framework(const char *id)
{
    log_debug("enter function %s", __FUNCTION__);
    framework_t *module = (framework_t*)load(ID_FRAMEWORK, id);
    module_t    *base = GET_MODULE(module);

    rpc_reg(base);
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
    log_init("framework");

    log_debug("enter function %s", __FUNCTION__);
    rpc_init(RPC_FRAMEWORK_NAME);

    // launch_framework("location");
    dictionary *inidict = iniparser_load(LOADER_CONF_PATH);
    if (!inidict) {
        log_debug("can't load '%s'", LOADER_CONF_PATH);
        exit(EXIT_FAILURE);
    }

    char buf[1024];
    strcpy(buf, iniparser_getstring(inidict, "loader:framework", NULL));
    str_t sbuf = str_new__cstr(buf);
    str_t *modules = str_split(sbuf, ",");
    str_t *p = modules;
    while (*p) {
        log_debug("module %s", *p);
        *p = str_trim(*p);
        launch_framework(*p);
        str_release(*p);
        p++;
    }
    free(modules);

    rpc_listen();

    log_debug("framework loader sleep...");
    while(1) {
        sleep(INT_MAX);
    }
    return 1;
}
