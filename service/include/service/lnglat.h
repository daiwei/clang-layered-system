#ifndef __SERVICE_LNGLAT_H__
#define __SERVICE_LNGLAT_H__

#include <common/def.h>
#include <service/location.h>

char lat_zone(double value);
char lng_zone(double value);
int lat_sign(char zone);
int lng_sign(char zone);

double lnglat_nmea_to_double(double value, char zone);
double lnglat_nmea_to_degree(double value);
double lnglat_nmea_to_minute(double value);
double lnglat_double_to_nmea(double value);
double lnglat_double_to_degree(double value);
double lnglat_double_to_minute(double value);

double distance(gps_location_t l1, gps_location_t l2);

#endif//__SERVICE_LNGLAT_H__
