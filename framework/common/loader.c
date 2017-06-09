#include <common/loader.h>
#include <common/util.h>

// static void *dyload(const char *module_id, const char *symbol)
// {
//     void *handle = NULL;
//
//     log_debug("dlopen %s", module_id);
//     handle = dlopen(module_id, RTLD_NOW);
//     if (handle == NULL) {
//         char const *err_str = dlerror();
//         log_debug("dlopen error: %s", err_str);
//         return NULL;
//     }
//
//     return dlsym(handle, symbol);
// }

static void *dlopen_module(const char* layer_id, const char *module_id)
{
    char path[1024];
    if (!strcmp(layer_id, ID_HARDWARE)) {
        sprintf(path, "hardware/lib/libhw.%s.so", module_id);
    } else if (!strcmp(layer_id, ID_FRAMEWORK)) {
        sprintf(path, "framework/lib/lib%s.so", module_id);
    } else if (!strcmp(layer_id, ID_SERVICE)) {
        sprintf(path, "service/lib/lib%s.so", module_id);
    } else if (!strcmp(layer_id, ID_APP)) {
        sprintf(path, "app/lib%s.so", module_id);    //TODO
    }

    void *handle = NULL;

    log_debug("dlopen %s", path);
    handle = dlopen(path, RTLD_NOW);
    if (handle == NULL) {
        char const *err_str = dlerror();
        log_debug("dlopen error: %s", err_str);
        return NULL;
    }

    return handle;
}

interface_t* import(module_t *module, const char* layer_id, const char *module_id)
{
    interface_t *iface = get_module_depend(module, layer_id, module_id);
    if (iface) {
        return iface;
    }

    log_debug("{{ import module %s **********", module_id);
    void *handle = dlopen_module(layer_id, module_id);

    dylib_init_t export_init = (module_t*)dlsym(handle, DYLIB_INIT_STR);
    if (export_init) export_init();

    iface = (interface_t*)dlsym(handle, INTERFACE_STR);
    if (iface == NULL) {
        log_debug("can not found %s", INTERFACE_STR);
        return NULL;
    }

    log_debug("}} import module %s **********", module_id);

    reg_module_depend(module, layer_id, module_id, iface);
    return iface;
}

module_t* load(const char* layer_id, const char *module_id)
{
    log_debug("{{ load module %s **********", module_id);
    void *handle = dlopen_module(layer_id, module_id);

    module_t *module = (module_t*)dlsym(handle, MODULE_STR);
    if (module == NULL) {
        log_debug("can not found %s", MODULE_STR);
        return NULL;
    }

    module->dlhandle = handle;

    dylib_init_t export_init = (module_t*)dlsym(handle, DYLIB_INIT_STR);
    if (export_init) export_init();
    if (module->info && module->info->on_load)  module->info->on_load();

    log_debug("}} load module %s **********", module_id);
    return module;
}

int unload(module_t *module)
{
    dlclose(module->dlhandle);
}
