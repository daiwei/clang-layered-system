#include <service/location.h>
#include <common/util.h>
#include <common/module.h>
#include <common/loader.h>
// #include <hardware/hardware.h>
// #include <hardware/gps.h>
#include <gps.h>

//=========================================================
// user-defined: data definition & function declaration
//=========================================================
static struct gps_data_t gpsdata;
static gps_location_t last_location;

static void process_loop();


//=========================================================
// module definition, will define `this` points to module
//=========================================================
MODULE_SERVICE("location");


//=========================================================
// module rpc method
//=========================================================
RPC_METHOD( WAIT_REPLY, get_current_location, 1,
            gps_location_t**,    OUT_SIZE);
int get_current_location(gps_location_t **location)
{
    rpc_arg_malloc(location, sizeof(gps_location_t));
    memcpy(*location, &last_location, sizeof(gps_location_t));
    return 0;
}

RPC_METHOD( WAIT_REPLY, rtc_calibration, 0);
int rtc_calibration()
{
    log_debug("enter function %s", __FUNCTION__);

    return 0;
}

EXPORT(location_interface_t, {
    RPC_REG(cam_get_support_config);
    RPC_REG(get_current_location);
    RPC_REG(rtc_calibration);
});


//=========================================================
// module state action
//=========================================================
static int start()
{
    log_debug("enter function %s", __FUNCTION__);

    // if (gps_open("localhost", DEFAULT_GPSD_PORT, &gpsdata) != 0) {
    //     log_error("cgps: no gpsd running or network error: %d, %s", errno, gps_errstr(errno));
    //     exit(2);
    // }
    //
    // unsigned int flags = WATCH_ENABLE;
    // (void)gps_stream(&gpsdata, flags, NULL);

    process_loop();
    return 0;
}

//=========================================================
// module load & unload method
//=========================================================
static int on_load()
{
    log_debug("enter function %s", __FUNCTION__);

    //-----------------------------------------------------
    // import module's dependent interface
    //-----------------------------------------------------

    //-----------------------------------------------------
    // init process after module loaded
    //-----------------------------------------------------
    this->start = start;

    return 0;
}

static int on_unload()
{
    return 0;
}


//=========================================================
// user-defined: function definition
//=========================================================
static void process_loop()
{
    log_debug("enter function %s", __FUNCTION__);
    while (true) {
        // if (!gps_waiting(&gpsdata, 5000000)) {
        //     log_error("gps wait time out.");
        // } else {
        //     errno = 0;
        //     if (gps_read(&gpsdata) == -1) {
        //         log_error("cgps: socket error 4");
        //     } else if (gpsdata.status == STATUS_FIX) {
        //             // last_location.flags = gpsdata.fix.;
        //             last_location.latitude = gpsdata.fix.latitude;
        //             last_location.longitude = gpsdata.fix.longitude;
        //             last_location.altitude = gpsdata.fix.altitude;
        //             last_location.speed = gpsdata.fix.speed;
        //             last_location.bearing = gpsdata.fix.track;
        //             // last_location.accuracy = gpsdata.fix.;
        //             last_location.timestamp = gpsdata.fix.time;
        //             // log_debug("update last_location timestamp:  %s", ctime(&last_location.timestamp));
        //     } else {
        //         log_debug("gps.status: %d", gpsdata.status);
        //         //TODO
        //     }
        // }
        sleep(10);
    }
}
