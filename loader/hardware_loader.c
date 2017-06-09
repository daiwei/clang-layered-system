#include <common/def.h>
#include <hardware/hardware.h>
#include <common/str.h>

int load(const char *id)
{
    hw_module_t *module;
    hw_get_module(id, &module);
    //TODO

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
    log_init("hardware");
}
