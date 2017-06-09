#ifndef __SERVICE_LOCALTION_H__
#define __SERVICE_LOCALTION_H__

#include <common/module.h>
// #include <hardware/gps.h>


typedef struct {
    int             flags;
    double          latitude;   // positive: N, negative: S
    double          longitude;  // positive: E, negative: W
    double          altitude;
    float           speed;
    float           bearing;
    float           accuracy;
    time_t          timestamp;
} gps_location_t;

typedef struct {

    int (*get_current_location)(gps_location_t **location);
    int (*rtc_calibration)();
    // int (*set_position_mode)(GpsPositionMode *mode, GpsPositionRecurrence *recurrence,
    //             uint32_t *min_interval, uint32_t *preferred_accuracy, uint32_t *preferred_time);

} location_interface_t;


#endif//__SERVICE_LOCALTION_H__
