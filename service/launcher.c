#include <common/str.h>
#include <common/loader.h>

// =========================================================
// user-defined: data definition & function declaration
// =========================================================
hash_t *module_process_map;

// =========================================================
// module definition, will define `this` points to module
// =========================================================
MODULE_SERVICE("launcher");
// EXPORT(tracker_interface_t);


// =========================================================
// module rpc method
// =========================================================
RPC_METHOD( WAIT_REPLY, launch_module, 2,
            char*,  MAX_ID,
            char*,  MAX_ID);
int launch_module(char* layer_id, char* module_id)
{
    log_debug("enter function %s", __FUNCTION__);

    app_t *app = (app_t*)load(layer_id, module_id);

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

        str_t *name = str_new__cstr(layer_id);
        name = str_append(name, "#", module_id);
        hash_set(module_process_map, name, &pid);
        str_release(name);

        return 1;
    } else {
        log_debug("app %s start error", info->module_id);
        return 0;
    }

    return 0;
}

RPC_METHOD( WAIT_REPLY, quit_module, 2,
            char*,  MAX_ID,
            char*,  MAX_ID);
int quit_module(char* layer_id, char* module_id)
{
    str_t *name = str_new__cstr(layer_id);
    name = str_append(name, "#", module_id);
    pid_t *pid = hash_get(module_process_map, name);
    str_release(name);

    kill(&pid, SIGTERM);
    return 0;
}


// =========================================================
// module state action
// =========================================================
static int start()
{
    log_debug("enter function %s", __FUNCTION__);

    if (!module_process_map) {
        module_process_map = hash_new();
    }

    launch_module();


    while (1) {
        sleep(INT_MAX);
    }

    return 0;
}


// =========================================================
// module load & unload method
// =========================================================
static int on_load()
{
    log_debug("enter function %s", __FUNCTION__);
    // -----------------------------------------------------
    // register module's rpc interface
    // -----------------------------------------------------


    // -----------------------------------------------------
    // import module's dependent interface
    // -----------------------------------------------------


    // -----------------------------------------------------
    // init process after module loaded
    // -----------------------------------------------------
    this->start = start;


    return 0;
}

static int on_unload()
{
    return 0;
}

// =========================================================
// user-defined: function definition
// =========================================================
