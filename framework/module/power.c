#include <framework/power.h>


//=========================================================
// user-defined: data definition & function declaration
//=========================================================


//=========================================================
// module definition, will define `this` points to module
//=========================================================
MODULE_FRAMEWORK("power");


//=========================================================
// module rpc method
//=========================================================
RPC_METHOD(WAIT_REPLY, workmode, 0);
int workmode()
{
    return 0;
}

RPC_METHOD(WAIT_REPLY, update_device, 0);
int update_device()
{
    return 0;
}

RPC_METHOD(WAIT_REPLY, sync_device, 0);
int sync_device()
{
    return 0;
}

RPC_METHOD(WAIT_REPLY, recover_device, 0);
int recover_device()
{
    return 0;
}

RPC_METHOD(WAIT_REPLY, reboot_device, 0);
int reboot_device()
{
    return 0;
}

RPC_METHOD(WAIT_REPLY, shutdown_device, 0);
int shutdown_device()
{
    return 0;
}

EXPORT(power_interface_t, {
    RPC_REG(workmode);
    RPC_REG(update_device);
    RPC_REG(sync_device);
    RPC_REG(recover_device);
    RPC_REG(reboot_device);
    RPC_REG(shutdown_device);
});

//=========================================================
// module state action
//=========================================================


//=========================================================
// module load & unload method
//=========================================================
static int on_load()
{
    //-----------------------------------------------------
    // import module's dependent interface
    //-----------------------------------------------------


    //-----------------------------------------------------
    // init process after module loaded
    //-----------------------------------------------------

    return 0;
}

static int on_unload()
{
    return 0;
}


//=========================================================
// user-defined: function definition
//=========================================================
