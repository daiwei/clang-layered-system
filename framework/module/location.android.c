#include <service/location.h>
#include <common/util.h>
#include <common/module.h>
#include <common/loader.h>
#include <hardware/hardware.h>
#include <hardware/gps.h>


//=========================================================
// user-defined: data definition & function declaration
//=========================================================
const GpsInterface *iface;
static GpsLocation last_location;
static GpsSvStatus last_sv_info;
static GpsStatus last_status;

static void gps_location_cb(GpsLocation* location);
static void gps_status_cb(GpsStatus* status);
static void gps_sv_status_cb(GpsSvStatus* sv_info);
static void gps_nmea_cb(GpsUtcTime timestamp, const char* nmea, int length);
static void gps_set_capabilities_cb(uint32_t capabilities);
static void gps_acquire_wakelock_cb();
static void gps_release_wakelock_cb();
static void gps_request_utc_time_cb();
static pthread_t gps_create_thread_cb(const char* name, void *(*start)(void *), void* arg);

static GpsCallbacks callbacks = {
    sizeof(GpsCallbacks),
    gps_location_cb,
    gps_status_cb,
    gps_sv_status_cb,
    gps_nmea_cb,
    gps_set_capabilities_cb,
    gps_acquire_wakelock_cb,
    gps_release_wakelock_cb,
    gps_create_thread_cb,
    gps_request_utc_time_cb
};


//=========================================================
// module definition, will define `this` points to module
//=========================================================
MODULE_FRAMEWORK("location");

// static tracker_interface_t *iface_tracker;

//=========================================================
// module rpc method
//=========================================================
RPC_METHOD(get_current_location, 1, GpsLocation*, sizeof(GpsLocation));
int get_current_location(GpsLocation *location)
{
    log_debug("enter function %s", __FUNCTION__);
    memcpy(location, &last_location, sizeof(GpsLocation));
    log_debug("timestamp:  %s", ctime(&location->timestamp));
    return 0;
}

RPC_METHOD(rtc_calibration, 0);
int rtc_calibration()
{
    log_debug("enter function %s", __FUNCTION__);

    return 0;
}

RPC_METHOD(set_position_mode, 5,
    GpsPositionMode*,       sizeof(GpsPositionMode),
    GpsPositionRecurrence*, sizeof(GpsPositionRecurrence),
    uint32_t*,              sizeof(uint32_t),
    uint32_t*,              sizeof(uint32_t),
    uint32_t*,              sizeof(uint32_t)
);
int set_position_mode(GpsPositionMode *mode, GpsPositionRecurrence *recurrence,
                uint32_t *min_interval, uint32_t *preferred_accuracy, uint32_t *preferred_time)
{
    log_debug("enter function %s", __FUNCTION__);

    // TODO
    iface->set_position_mode(*mode, *recurrence, *min_interval, *preferred_accuracy, *preferred_time);

    return 0;
}

EXPORT(location_interface_t, {
    RPC_REG(get_current_location);
    RPC_REG(rtc_calibration);
    RPC_REG(set_position_mode);
});


//=========================================================
// module state action
//=========================================================


//=========================================================
// module load & unload method
//=========================================================
static int on_load()
{
    log_debug("enter function %s", __FUNCTION__);

    //-----------------------------------------------------
    // import module's dependent interface
    //-----------------------------------------------------
    // iface_tracker = IMPORT_SERVICE("tracker");
    // if (!iface_tracker) {
    //     log_debug("import %s error", "tracker");
    // }

    //-----------------------------------------------------
    // init process after module loaded
    //-----------------------------------------------------
    // call android hal gps module
    struct hw_module_t *gpsm;
    struct gps_device_t *gps;

    hw_get_module("gps", &gpsm);
    gpsm->methods->open(gpsm, gpsm->name, &gps);

    iface = gps->get_gps_interface(gps);
    iface->init(&callbacks);

    return 0;
}

static int on_unload()
{
    return 0;
}


//=========================================================
// user-defined: function definition
//=========================================================
// Android HAL GPS Interface
static void gps_location_cb(GpsLocation* location)
{
    log_debug("enter function %s", __FUNCTION__);
    if (!location) {
        log_debug("arg is null");
        return;
    }

    // log_debug("flags:      %d", location->flags);
    // log_debug("latitude:   %f", location->latitude);
    // log_debug("longitude:  %f", location->longitude);
    // log_debug("altitude:   %f", location->altitude);
    // log_debug("speed:      %f", location->speed);
    // log_debug("bearing:    %f", location->bearing);
    // log_debug("accuracy:   %f", location->accuracy);
    log_debug("timestamp:  %s", ctime(&location->timestamp));

    memcpy(&last_location, location, sizeof(GpsLocation));
    // iface_tracker->gps_location_cb(location);
}

static void gps_status_cb(GpsStatus* status)
{
    log_debug("enter function %s", __FUNCTION__);
    memcpy(&last_status, status, sizeof(GpsStatus));
    // iface_tracker->gps_status_cb(status);
}

static void gps_sv_status_cb(GpsSvStatus* sv_info)
{
    log_debug("enter function %s", __FUNCTION__);
    memcpy(&last_sv_info, sv_info, sizeof(GpsSvStatus));
}

static void gps_nmea_cb(GpsUtcTime timestamp, const char* nmea, int length)
{
    log_debug("enter function %s", __FUNCTION__);
}

static void gps_set_capabilities_cb(uint32_t capabilities)
{
    log_debug("enter function %s", __FUNCTION__);
}

static void gps_acquire_wakelock_cb()
{
    log_debug("enter function %s", __FUNCTION__);
}

static void gps_release_wakelock_cb()
{
    log_debug("enter function %s", __FUNCTION__);
}

static void gps_request_utc_time_cb()
{
    log_debug("enter function %s", __FUNCTION__);
}

static pthread_t gps_create_thread_cb(const char* name, void *(*start)(void *), void* arg)
{
    log_debug("enter function %s", __FUNCTION__);
    pthread_t tid;
    int ret = pthread_create(&tid, NULL, start, arg);
    if (ret != 0) {
        log_debug("create pthread error!");
    }

    return tid;
}
