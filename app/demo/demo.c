#include "demo.h"
#include <common/def.h>
#include <service/location.h>
#include <common/loader.h>
#include <common/module.h>
#include <common/util.h>

//=========================================================
// user-defined: data definition & function declaration
//=========================================================


//=========================================================
// module definition, will define `this` points to module
//=========================================================
MODULE_APP("demo");

location_interface_t *iface_location;

//=========================================================
// module state action
//=========================================================
static int demo_init()
{
    log_debug("enter function %s", __FUNCTION__);
    return 0;
}

static int demo_start()
{
    log_debug("enter function %s", __FUNCTION__);
    foo();
    return 0;
}

static int demo_pause()
{
    log_debug("enter function %s", __FUNCTION__);
    return 0;
}

static int demo_restart()
{
    log_debug("enter function %s", __FUNCTION__);
    return 0;
}

static int demo_stop()
{
    log_debug("enter function %s", __FUNCTION__);
    return 0;
}

static int demo_destroy()
{
    log_debug("enter function %s", __FUNCTION__);
    return 0;
}

//=========================================================
// module load & unload method
//=========================================================
static int on_load()
{
    //-----------------------------------------------------
    // register module's rpc interface
    //-----------------------------------------------------

    //-----------------------------------------------------
    // import module's dependent interface
    //-----------------------------------------------------
    iface_location = IMPORT_FRAMEWORK("location");

    //-----------------------------------------------------
    // init process after module loaded
    //-----------------------------------------------------
    this->init  = demo_init;
    this->start = demo_start;

    return 0;
}

static int on_unload()
{
    return 0;
}


//=========================================================
// user-defined: function definition
//=========================================================
void foo()
{
    log_debug("enter function %s", __FUNCTION__);
    while (1) {
        log_debug("call location->rtc_calibration");
        iface_location->rtc_calibration();
        sleep(5);
    }

    // GpsLocation gpslocal;
    //
    // int run_times = 1;
    // time_t start = time(NULL);
    // for (int i = 0; i < run_times; i++) {
    //     iface_location->get_current_location(&gpslocal);
    // }
    // time_t end = time(NULL);
    // log_debug(">>>dbus call %d times. start: %ld, end: %ld", run_times, start, end);

    log_debug("leave function %s", __FUNCTION__);
}
