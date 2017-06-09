/*
 * hardware/fastrax_gps/fastrax_gps.c
 *
 * Inspired by the Freerunner GPS module by Michael Trimarchi.
 *
 * Copyright 2012, Boundary Devices <info@boundarydevices.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// #define LOG_TAG "fastrax_gps"
// #include <cutils/log.h>
#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <hardware/gps.h>
#include <hardware/minmea.h>

#define DISABLE_CLEANUP 1
// #define GPS_DEBUG
// #define GPS_LOG_DATA
#define FAKEDATA        0

#define ARRAY_SIZE(__arr) (sizeof(__arr) / sizeof(__arr[0]))

typedef struct {
    GpsInterface base;
    GpsCallbacks callbacks;
    pthread_t reader;
    int readersock[2];
    int running;
    int active;
    GpsLocation fix;
    GpsSvStatus sv_status;
    int sv_status_changed;
} FastraxGpsInterface;

static int init(GpsCallbacks* callbacks);
static int start(void);
static int stop(void);
static void cleanup(void);
static int inject_time(GpsUtcTime t, int64_t timeReference, int uncertainty);
static int inject_location(double latitude, double longitude, float accuracy);
static void delete_aiding_data(GpsAidingData flags);
static int set_position_mode(GpsPositionMode mode, GpsPositionRecurrence recurrence,
                             uint32_t min_interval,
                             uint32_t preferred_accuracy,
                             uint32_t preferred_time);
static const void* get_extension(const char* name);

static FastraxGpsInterface dev = {
    .base = {
        .size = sizeof(FastraxGpsInterface),
        .init = init,
        .start = start,
        .stop = stop,
        .cleanup = cleanup,
        .inject_time = inject_time,
        .inject_location = inject_location,
        .delete_aiding_data = delete_aiding_data,
        .set_position_mode = set_position_mode,
        .get_extension = get_extension
    },
    .readersock = { 0, 0 },
    .running = 0,
    .sv_status_changed = 0
};

#define MAX_TOKENS  32
#define MAX_LINELEN 256

#ifdef GPS_DEBUG
static void print_tokens(char** tokens, unsigned count)
{
    char linebuf[MAX_LINELEN + 1];
    char* nextout = linebuf;

    while (count--) {
        nextout += snprintf(nextout,
                            linebuf + sizeof(linebuf) - nextout - 1,
                            "%s|", *tokens++);
    }
    *nextout++ = '\n';
    *nextout = '\0';
    log_debug("%s", linebuf);
}
#endif

/* fill in tm_hour, tm_min, tm_sec,
 * return zero for success
 */
static int parse_hms(char const* tok, struct tm* tm)
{
    if (6 <= strlen(tok)) {
        int i;
        unsigned hms[3] = { 0, 0, 0 };
        char const* next = tok;
        for (i = 0; i < 3; i++) {
            hms[i] = ((next[0] - '0') * 10)
                     + (next[1] - '0');
            next += 2;
        }
        if ((24 > hms[0])
            && (60 > hms[1])
            && (60 > hms[2])) {
            tm->tm_hour = hms[0];
            tm->tm_min = hms[1];
            tm->tm_sec = hms[2];
            return 0;
        } else {
            log_debug("hms: %d/%d/%d", hms[0], hms[1], hms[2]);
        }
    }
    log_debug("Invalid hms: %s", tok);
    return -1;
}

static int parse_dmy(char const* tok, struct tm* tm)
{
    if (6 <= strlen(tok)) {
        int i;
        unsigned dmy[3] = { 0, 0, 0 };
        char const* next = tok;
        for (i = 0; i < 3; i++) {
            dmy[i] = ((next[0] - '0') * 10)
                     + (next[1] - '0');
            next += 2;
        }
        if ((32 > dmy[0])
            && (13 > dmy[1])
            && (12 <= dmy[2])
            && (99 > dmy[2])) {
            tm->tm_mday = dmy[0];
            tm->tm_mon = dmy[1] - 1;
            tm->tm_year = dmy[2] + 2000 - 1900;
            return 0;
        } else {
            log_debug("dmy: %d/%d/%d", dmy[0], dmy[1], dmy[2]);
        }
    }
    log_debug("Invalid dmy: %s", tok);
    return -1;
}

static int parse_hhmm(char const* tok, double* v)
{
    double val = strtod(tok, 0);
    int degrees = (int)(floor(val) / 100);
    double minutes = val - degrees * 100.;

    *v = degrees + minutes / 60.0;
    return 0;
}

/*
 * Adjust time to local time and convert to ms
 */
static void update_time(GpsLocation* fix, struct tm* tmgps)
{
    struct tm tmutc, tmlocal;
    time_t fix_time, wall, gps;
    int utc_offs;

    wall = time(0);
    gmtime_r(&wall, &tmutc);
    localtime_r(&wall, &tmlocal);
    utc_offs = mktime(&tmlocal) - wall;
    gps = mktime(tmgps) + utc_offs;
    fix->timestamp = (long long)gps * 1000;

    log_debug("UTC %04u-%02u-%02u %02u:%02u:%02u",
           tmutc.tm_year + 1900, tmutc.tm_mon + 1, tmutc.tm_mday,
           tmutc.tm_hour, tmutc.tm_min, tmutc.tm_sec);
    log_debug("loc %04u-%02u-%02u %02u:%02u:%02u",
           tmlocal.tm_year + 1900, tmlocal.tm_mon + 1, tmlocal.tm_mday,
           tmlocal.tm_hour, tmlocal.tm_min, tmlocal.tm_sec);
    log_debug("gps %04u-%02u-%02u %02u:%02u:%02u",
           tmgps->tm_year + 1900, tmgps->tm_mon + 1, tmgps->tm_mday,
           tmgps->tm_hour, tmgps->tm_min, tmgps->tm_sec);
    log_debug("utc_offs: %d, gps: %ld, timestamp %ld",
           utc_offs, gps, fix->timestamp);
}

static void process_line(FastraxGpsInterface* iface, char* line, unsigned len)
{
    log_debug("enter function %s", __FUNCTION__);

    char* const end = line + len;
    char* tokens[MAX_TOKENS + 1];
    char* prevtok = line;
    unsigned num_tokens = 0;
    *end = 0;
    log_debug("%d: %s", len, line);
    tokens[0] = line;
    while (line < end) {
        char const c = *line++;
        if (',' == c) {
            line[-1] = '\0';
            tokens[++num_tokens] = prevtok = line;
            if (MAX_TOKENS == num_tokens) {
                break;
            }
        }
    }
    if (line > prevtok)
        ++num_tokens;

    if (0 < num_tokens) {
        char const* last = tokens[num_tokens - 1];
        char* cksum;
        if (0 != (cksum = strchr(last, '*')))
            *cksum = 0;
    }

    log_debug("%s: %u tokens", __FUNCTION__, num_tokens);
#ifdef GPS_DEBUG
    print_tokens(tokens, num_tokens);
#endif

    if (0 < num_tokens) {
        char const* const nmea = tokens[0];
        if (!strcmp("$GPGGA", nmea)) {
            if (6 <= num_tokens) {
                // GPS fix: http://aprs.gids.nl/nmea/#gga
                if ('0' < *tokens[6]) {
                    /*
                       0    1         23456 7 89a
                       $GPGGA,000223.036,,,,,0,0,,,M,,M,,*4E
                     */
                    struct tm tm;
                    double latitude, longitude;
                    memset(&tm, 0, sizeof(tm));
                    log_debug("fix %c", *tokens[6]);
                    if ((0 == parse_hms(tokens[1], &tm))
                        && (0 == parse_hhmm(tokens[2], &latitude))
                        && (0 == parse_hhmm(tokens[4], &longitude))) {
                        double altitude = 0.0;
                        if (*tokens[9]) {
                            altitude = strtod(tokens[9], 0);
                            iface->fix.flags |= GPS_LOCATION_HAS_ALTITUDE;
                            iface->fix.altitude = altitude;
                        }
                        if ('S' == *tokens[3])
                            latitude = -latitude;
                        if ('W' == *tokens[5])
                            longitude = -longitude;
                        iface->fix.flags |= GPS_LOCATION_HAS_LAT_LONG;
                        iface->fix.latitude = latitude;
                        iface->fix.longitude = longitude;
                        log_debug("fix data: time %2u:%2u:%2u, lat %lf, long %lf, alt %lf%c",
                               tm.tm_hour, tm.tm_min, tm.tm_sec,
                               latitude, longitude, altitude, *tokens[10]);
                    } else {
                        log_debug("Error parsing fix data");
                    }
                } else {
                    log_debug("no fix");
                }
            } else {
                log_debug("bad data for %s", nmea);
            }
        } else if (!strcmp("$GPGSA", nmea)) {
            /*
               0      1 2 3456789abcdef01
               $GPGSA,A,1,,,,,,,,,,,,,,,*1E
             */
            if (15 < num_tokens) {
                char const fix = *tokens[2];
                if (('\0' != fix) && ('1' != fix)) {
                    double accuracy = strtod(tokens[15], 0);
                    unsigned mask = 0;
                    int i;
                    iface->fix.accuracy = accuracy;
                    iface->fix.flags |= GPS_LOCATION_HAS_ACCURACY;
                    log_debug("%s - have fix: accuracy %lf", nmea, accuracy);
                    for (i = 3; i < 14; i++) {
                        unsigned val = strtoul(tokens[i], 0, 0);
                        if (val) {
                            mask |= 0x80000000 >> (i - 3);
                        }
                    }
                    log_debug("satellite mask: %08x", mask);
                    iface->sv_status.used_in_fix_mask = mask;
                    iface->sv_status_changed = 1;
                } else {
                    log_debug("%s - no fix", nmea);
                }
            } else {
                log_debug("bad data for %s", nmea);
            }
        } else if (!strcmp("$GPGSV", nmea)) {
            if (4 <= num_tokens) {
                /*
                   0      1 2 3456789abcdef01
                   $GPGSV,1,1,00*79
                 */
                int num_sat = strtoul(tokens[3], 0, 0);
                log_debug("%u satellites", num_sat);
                if (0 < num_sat) {
                    unsigned num_sent = strtoul(tokens[1], 0, 0);
                    unsigned sent = strtoul(tokens[2], 0, 0);
                    int i, curr;
                    if (1 == sent) {
                        iface->sv_status.num_svs = 0;
                        iface->sv_status_changed = 0;
                    }
                    curr = iface->sv_status.num_svs;
                    for (i = 0; (i < 4) && (iface->sv_status.num_svs < num_sat); i++) {
                        GpsSvInfo* sv = iface->sv_status.sv_list + curr;
                        sv->prn = strtoul(tokens[(i * 4) + 4], 0, 0);
                        sv->elevation = strtod(tokens[(i * 4) + 5], 0);
                        sv->azimuth = strtod(tokens[(i * 4) + 6], 0);
                        sv->snr = strtod(tokens[(i * 4) + 7], 0);
                        log_debug("sat[%u]: prn:%u, el:%lf, az:%lf, snr:%lf",
                               curr, sv->prn, sv->elevation, sv->azimuth, sv->snr);
                        iface->sv_status.num_svs++;
                        curr++;
                        sent++;
                    }
                    if (sent >= num_sent) {
                        iface->sv_status_changed = 1;
                    }  /* done with GPGSV for this cycle */
                }
            } else {
                log_debug("bad data for %s", nmea);
            }
        } else if (!strcmp("$GPRMC", nmea)) {
            if (10 <= num_tokens) {
                // http://aprs.gids.nl/nmea/#rmc
                if ('A' == *tokens[2]) {
                    struct tm tm;
                    double latitude, longitude;
                    memset(&tm, 0, sizeof(tm));
                    if ((0 == parse_hms(tokens[1], &tm))
                            && (0 == parse_hhmm(tokens[3], &latitude))
                            && (0 == parse_hhmm(tokens[5], &longitude))
                            && (0 == parse_dmy(tokens[9], &tm)))
                    {
                        double speed, bearing;
                        unsigned date;
                        speed = strtod(tokens[7], 0);
                        bearing = strtod(tokens[8], 0);
                        if ('S' == *tokens[4])
                            latitude = -latitude;
                        if ('W' == *tokens[6])
                            longitude = -longitude;
                        iface->fix.bearing = bearing;
                        iface->fix.flags |= GPS_LOCATION_HAS_LAT_LONG
                                            | GPS_LOCATION_HAS_BEARING
                                            | GPS_LOCATION_HAS_SPEED;
                        log_debug("At %lf:%lf, speed %lf, bearing %lf, @%04u-%02u-%02u %02u:%02u:%02u",
                               latitude, longitude, speed, bearing,
                               tm.tm_year + 1900,
                               tm.tm_mon + 1,
                               tm.tm_mday,
                               tm.tm_hour,
                               tm.tm_min,
                               tm.tm_sec);
                        update_time(&iface->fix, &tm);
                    } else {
                        log_debug("Invalid date: %s", tokens[1]);
                    }
                } else {
                    log_debug("bad fix for %s", nmea);
                }
            } else {
                log_debug("bad data for %s", nmea);
            }
        } else {
            log_debug("unknown NMEA sentence: %s", nmea);
        }
    }

    log_debug("leave function %s", __FUNCTION__);
}

static char const* const fake_messages[] = {
    "$GPGSV,3,1,12,04,56,158,31,17,55,031,,27,51,300,,28,41,085,*75",
    "$GPGSV,3,2,12,09,40,310,,02,26,193,40,08,13,153,17,15,13,255,*72",
    "$GPGSV,3,3,12,26,11,216,21,12,07,299,,20,06,068,,01,01,035,*72",
    // "$GPRMC,210432.000,A,3318.6418,N,11157.9907,W,0.88,282.15,140812,,,A*7E",
    "$GPRMC,210432.000,A,3318.6418,N,11157.9907,W,0.88,282.15,060616,,,A",
    "$GPGGA,210433.000,3318.6422,N,11157.9904,W,1,4,4.01,400.2,M,-26.8,M,,*63",
    "$GPGSA,A,3,04,26,02,08,,,,,,,,,4.13,4.01,0.97*05",
    "$GPGSV,3,1,12,04,56,158,31,17,55,031,,27,51,300,,28,41,085,*75",
    "$GPGSV,3,2,12,09,40,310,,02,26,193,40,08,13,153,16,15,13,255,*73",
    "$GPGSV,3,3,12,26,11,216,21,12,07,299,,20,06,068,,01,01,035,*72",
    "$GPRMC,210433.000,A,3318.6422,N,11157.9904,W,0.15,282.15,140812,,,A*71",
    "$GPGGA,210434.000,3318.6428,N,11157.9913,W,1,4,4.01,400.3,M,-26.8,M,,*69",
    "$GPGSA,A,3,04,26,02,08,,,,,,,,,4.13,4.01,0.97*05",
    "$GPGSV,3,1,12,04,56,158,31,17,55,031,,27,51,300,,28,41,085,*75",
    "$GPGSV,3,2,12,09,40,310,,02,26,193,40,08,13,153,16,15,13,255,*73",
    "$GPGSV,3,3,12,26,11,216,22,12,07,299,,20,06,068,,01,01,035,*71",
    "$GPRMC,210434.000,A,3318.6428,N,11157.9913,W,2.34,285.77,140812,,,A*78",
    "$GPGGA,210435.000,3318.6430,N,11157.9917,W,1,4,4.01,400.2,M,-26.8,M,,*64",
    "$GPGSA,A,3,04,26,02,08,,,,,,,,,4.12,4.01,0.97*04",
    "$GPGSV,3,1,12,04,56,158,30,17,55,031,,27,51,300,,28,41,085,*74",
    "$GPGSV,3,2,12,09,40,310,,02,26,193,40,08,13,153,16,15,13,255,*73",
    "$GPGSV,3,3,12,26,11,216,22,12,07,299,,20,06,068,,01,01,035,*71",
    "$GPRMC,210435.000,A,3318.6430,N,11157.9917,W,2.67,288.80,140812,,,A*77",
    "$GPGGA,210436.000,3318.6437,N,11157.9914,W,1,4,4.01,400.1,M,-26.8,M,,*60",
    "$GPGSA,A,3,04,26,02,08,,,,,,,,,4.12,4.01,0.97*04",
    "$GPGSV,3,1,12,04,56,158,30,17,55,031,,27,51,300,,28,41,085,*74",
    "$GPGSV,3,2,12,09,40,310,,02,26,193,40,08,13,153,16,15,13,255,*73",
    "$GPGSV,3,3,12,26,11,216,22,12,07,299,,20,06,068,,01,01,035,*71",
    "$GPRMC,210436.000,A,3318.6437,N,11157.9914,W,1.30,291.40,140812,,,A*75",
    "$GPGGA,210437.000,3318.6440,N,11157.9912,W,1,4,4.01,400.0,M,-26.8,M,,*66",
    "$GPGSA,A,3,04,26,02,08,,,,,,,,,4.12,4.01,0.97*04",
    "$GPGSV,3,1,11,04,56,158,30,17,55,031,,27,51,300,,28,41,085,*77",
    "$GPGSV,3,2,11,09,40,310,,02,26,193,40,08,13,153,16,15,13,255,*70",
    "$GPGSV,3,3,11,26,11,216,22,12,07,299,,20,06,068,*44",
    "$GPRMC,210437.000,A,3318.6440,N,11157.9912,W,0.97,341.12,140812,,,A*75",
    "$GPGGA,210438.000,3318.6433,N,11157.9904,W,1,4,4.01,400.1,M,-26.8,M,,*6B",
    "$GPGSA,A,3,04,26,02,08,,,,,,,,,4.13,4.01,0.97*05",
    "$GPGSV,3,1,11,04,56,158,30,17,55,031,,27,51,300,,28,41,085,*77",
    "$GPGSV,3,2,11,09,40,310,,02,26,193,40,08,13,153,16,15,13,255,*70",
    "$GPGSV,3,3,11,26,11,216,22,12,07,299,,20,06,068,*44",
    "$GPRMC,210438.000,A,3318.6433,N,11157.9904,W,0.80,63.28,140812,,,A*45",
    "$GPGGA,210439.000,3318.6435,N,11157.9901,W,1,4,4.00,399.9,M,-26.8,M,,*67",
    "$GPGSA,A,3,04,26,02,08,,,,,,,,,4.12,4.00,0.97*05",
    "$GPGSV,3,1,11,04,56,158,30,17,55,031,,27,51,300,,28,41,085,*77",
    "$GPGSV,3,2,11,09,40,310,,02,26,193,40,08,13,153,16,15,13,255,*70",
    "$GPGSV,3,3,11,26,11,216,22,12,07,299,,20,06,068,*44",
    "$GPRMC,210439.000,A,3318.6435,N,11157.9901,W,0.75,61.59,140812,,,A*49",
    "$GPGGA,210440.000,3318.6445,N,11157.9905,W,1,4,4.01,399.7,M,-26.8,M,,*65",
    "$GPGSA,A,3,04,26,02,08,,,,,,,,,4.12,4.01,0.97*04",
    "$GPGSV,3,1,11,04,56,158,30,17,55,031,,27,51,300,,28,41,085,*77",
    "$GPGSV,3,2,11,09,40,310,,02,26,193,40,08,13,153,16,15,13,255,*70",
    "$GPGSV,3,3,11,26,11,216,22,12,07,299,,20,06,068,*44",
    "$GPRMC,210440.000,A,3318.6445,N,11157.9905,W,0.39,61.59,140812,,,A*4C",
    "$GPGGA,210441.000,3318.6447,N,11157.9906,W,1,4,4.00,399.5,M,-26.8,M,,*66",
    "$GPGSA,A,3,04,26,02,08,,,,,,,,,4.12,4.00,0.97*05",
    "$GPGSV,3,1,11,04,56,158,30,17,55,031,,27,51,300,,28,41,085,*77",
    "$GPGSV,3,2,11,09,40,310,,02,26,193,40,08,13,153,17,15,13,255,*71",
    "$GPGSV,3,3,11,26,11,216,22,12,07,299,,20,06,068,*44"
};

static char const* findFake(char const* line)
{
    static int fake_idx = 0;
    char const* comma = strchr((char*)line, ',');
    if (comma) {
        int const start = fake_idx;
        unsigned len = comma - line;
        do {
            char const* fm = fake_messages[fake_idx];
            fake_idx = (fake_idx + 1) % ARRAY_SIZE(fake_messages);
            if (0 == memcmp(line, fm, len)) {
                return fm;
            } else {
                fake_idx = (fake_idx + 1) % ARRAY_SIZE(fake_messages);
            }
        } while (fake_idx != start);
    }
    return line;
}

#if 0
static void fastrax_reader(void* arg)
{
    log_debug("enter function %s", __FUNCTION__);
    int retval;
    int fdserial;
    struct pollfd fds[2];
    struct termios serialstate;
    char linebuf[MAX_LINELEN];
    int start = 0;
    char* const eol = linebuf + sizeof(linebuf) - 1;

    dev.running = 1;
    fdserial = open("/dev/ttySC0", O_RDWR);
    if (0 > fdserial) {
        perror("/dev/ttySC0");
        return;
    }
    retval = fcntl(fdserial, F_SETFD, FD_CLOEXEC);
    retval = fcntl(fdserial, F_SETFL, O_NONBLOCK);

    tcgetattr(fdserial, &serialstate);
    serialstate.c_cc[VMIN] = 1;
    cfsetispeed(&serialstate, B9600);
    cfsetospeed(&serialstate, B9600);
    serialstate.c_cc[VTIME] = 0;  // 1/10th's of a second, see http://www.opengroup.org/onlinepubs/007908799/xbd/termios.html
    serialstate.c_cflag &= ~(PARENB | CSTOPB | CSIZE | CRTSCTS);  // Mask character size to 8 bits, no parity, Disable hardware flow control
    serialstate.c_cflag |= (CLOCAL | CREAD | CS8);  // Select 8 data bits
    serialstate.c_lflag &= ~(ICANON | ECHO);  // set raw mode for input
    serialstate.c_iflag &= ~(IXON | IXOFF | IXANY | INLCR | ICRNL | IUCLC);  // no software flow control
    serialstate.c_oflag &= ~OPOST;  // raw output
    tcsetattr(fdserial, TCSANOW, &serialstate);

    fds[0].fd = fdserial;
    fds[0].events = POLLIN;
    fds[1].fd = dev.readersock[1];
    fds[1].events = POLLIN;
    if (dev.callbacks.status_cb) {
        GpsStatus gps_status;
        gps_status.size = sizeof(gps_status);
        gps_status.status = GPS_STATUS_SESSION_BEGIN;
        log_debug("SESSION_BEGIN");
        dev.callbacks.status_cb(&gps_status);
    }
    do {
        int nfds = poll(fds, ARRAY_SIZE(fds), 1000);
        if (0 < nfds) {
            if (fds[1].revents) {
                log_debug("%s close request", __FUNCTION__);
                break;
            }
            if (fds[0].revents) {
                char* next = linebuf + start;
                int numread = read(fdserial, next, eol - next);
                if (0 <= numread) {
                    int i = 0;
                    for (i = 0; i < numread; i++) {
                        char const c = *next;
                        if ('\r' == c) {
                            if (FAKEDATA && (0 == strncmp("$GP", linebuf, 3))) {
                                char temp[256];
                                strcpy(temp, findFake(linebuf));
                                process_line(&dev, temp, strlen(temp));
                            } else {
                                process_line(&dev, linebuf, next - linebuf);
                            }
                            memcpy(linebuf, next + 1, numread - i - 1);
                            next = linebuf;
                        } else if (('\n' == c)
                                   && (next == linebuf)) {
                            memcpy(linebuf, next + 1, numread - i - 1);
                        } else {
                            next++;
                        }
                    }
                    start = next - linebuf;
                    /* log_debug("%s:%s %d bytes leftover", __FILE__, __FUNCTION__, start); */
                    if (sizeof(linebuf) == start) {
                        log_debug("%s line buffer overflow", __FUNCTION__);
                        start = 0;
                    }
                } else {
                    break;
                    log_debug("%s: read error %d", __FUNCTION__, numread);
                }
            }
        } else {
            log_debug("%s: idle", __FUNCTION__);
        }

        if (dev.fix.flags & GPS_LOCATION_HAS_LAT_LONG) {
            if (dev.callbacks.location_cb) {
                log_debug(">>>>>>>>>>>> issue location callback");
                dev.callbacks.location_cb(&dev.fix);
                dev.fix.flags = 0;
            } else {
                log_debug("!!!!!!!!! no location callback");
            }
        }
        if (dev.sv_status_changed) {
            log_debug(">>>>>>>>>>>>>> sv status callback");
            if (dev.callbacks.sv_status_cb) {
                dev.sv_status_changed = 0;
                dev.callbacks.sv_status_cb(&dev.sv_status);
            }
        }
    } while (1);

    log_debug("%s - exit", __FUNCTION__);
    close(dev.readersock[0]);
    dev.readersock[0] = -1;
    close(dev.readersock[1]);
    dev.readersock[1] = -1;
    dev.running = 0;

    return;
}
#else
static void fastrax_reader(void* arg)
{
    log_debug("enter function %s", __FUNCTION__);
    int retval;
    char linebuf[MAX_LINELEN];
    int start = 0;
    char* const eol = linebuf + sizeof(linebuf) - 1;

    dev.running = 1;
    // if (dev.callbacks.status_cb) {
    // GpsStatus gps_status;
    // gps_status.size = sizeof(gps_status);
    // gps_status.status = GPS_STATUS_SESSION_BEGIN;
    // log_debug("SESSION_BEGIN");
    // dev.callbacks.status_cb(&gps_status);
    // }

    do {
        strcpy(linebuf, "$GPRMC,");
        char temp[256];
        strcpy(temp, findFake(linebuf));
        log_debug("line: %s", temp);
        // process_line(&dev, temp, strlen(temp));

        switch (minmea_sentence_id(temp, false)) {
            case MINMEA_SENTENCE_RMC: {
                struct minmea_sentence_rmc frame;
                if (minmea_parse_rmc(&frame, temp)) {
                    dev.fix.size        = sizeof(GpsLocation);
                    dev.fix.flags       = 0x0F;
                    dev.fix.latitude    = minmea_tocoord(&frame.latitude);
                    dev.fix.longitude   = minmea_tocoord(&frame.longitude);
                    dev.fix.altitude    = 0.0;
                    dev.fix.speed       = minmea_tofloat(&frame.speed);
                    dev.fix.bearing     = 0.0;
                    dev.fix.accuracy    = 0.0;

                    struct timespec ts;
                    memset(&ts, 0, sizeof(ts));
                    minmea_gettime(&ts, &frame.date, &frame.time);
                    log_debug("Y%d M%d D%d", frame.date.year, frame.date.month, frame.date.day);
                    dev.fix.timestamp = ts.tv_sec;
                    log_debug("timestamp: %d", dev.fix.timestamp);

                    // log_debug("$RMC: raw coordinates and speed: (%d/%d,%d/%d) %d/%d",
                    //         frame.latitude.value, frame.latitude.scale,
                    //         frame.longitude.value, frame.longitude.scale,
                    //         frame.speed.value, frame.speed.scale);
                    // log_debug("$RMC fixed-point coordinates and speed scaled to three decimal places: (%d,%d) %d",
                    //         minmea_rescale(&frame.latitude, 1000),
                    //         minmea_rescale(&frame.longitude, 1000),
                    //         minmea_rescale(&frame.speed, 1000));
                    // log_debug("$RMC floating point degree coordinates and speed: (%f,%f) %f",
                    //         minmea_tocoord(&frame.latitude),
                    //         minmea_tocoord(&frame.longitude),
                    //         minmea_tofloat(&frame.speed));
                }
            } break;

            case MINMEA_SENTENCE_GGA: {
                struct minmea_sentence_gga frame;
                if (minmea_parse_gga(&frame, temp)) {
                    log_debug("$GGA: fix quality: %d\n", frame.fix_quality);
                }
            } break;

            default :
                log_debug("%d", minmea_sentence_id(temp, false));
                break;
        }

        if (dev.fix.flags & GPS_LOCATION_HAS_LAT_LONG) {
            if (dev.callbacks.location_cb) {
                log_debug(">>>>>>>>>>>> issue location callback");
                dev.callbacks.location_cb(&dev.fix);
                dev.fix.flags = 0;
            } else {
                log_debug("!!!!!!!!! no location callback");
            }
        }

        if (dev.sv_status_changed) {
            log_debug(">>>>>>>>>>>>>> sv status callback");
            if (dev.callbacks.sv_status_cb) {
                dev.sv_status_changed = 0;
                dev.callbacks.sv_status_cb(&dev.sv_status);
            }
        }

        sleep(3);
    } while (1);

    log_debug("%s - exit", __FUNCTION__);
    dev.running = 0;

    return;
}
#endif

static int init(GpsCallbacks* callbacks)
{
    memset(&dev.fix, 0, sizeof(dev.fix));
    memset(&dev.sv_status, 0, sizeof(dev.sv_status));
    log_debug("%s:%p", __FUNCTION__, callbacks);
    dev.callbacks = *callbacks;
    if (0 == dev.running) {
        int retval;
        log_debug("%s:not running, start reader", __FUNCTION__);
        retval = socketpair(AF_LOCAL, SOCK_DGRAM, 0, dev.readersock);
        log_debug("%s:socket %d", __FUNCTION__, retval);
        callbacks->create_thread_cb("loc_api", fastrax_reader, &dev);
        log_debug("%s:started: thread %ld", __FUNCTION__, dev.reader);
    }
    return 0;
}

static int start(void)
{
    log_debug("%s", __FUNCTION__);
    return 0;
}

static int stop(void)
{
    log_debug("%s", __FUNCTION__);
    return 0;
}

static void cleanup(void)
{
    log_debug("%s", __FUNCTION__);
#ifdef DISABLE_CLEANUP
    log_debug("%s disabled at compile time", __FUNCTION__);
#else
    if (0 != dev.running) {
        int retval;
        int dummy = 0;
        void* exitstat = (void*)-1;
        log_debug("%s:running: stop reader %d here", __FUNCTION__, dev.reader);
        retval = write(dev.readersock[0], &dummy, 1);
        log_debug("%s:shutdown: %d", __FUNCTION__, retval);
        retval = pthread_join(dev.reader, &exitstat);
        log_debug("%s:running: join %p", __FUNCTION__, exitstat);
    } else {
        log_debug("%s:not running", __FUNCTION__);
    }
#endif
}

static int inject_time(GpsUtcTime t, int64_t timeReference, int uncertainty)
{
    log_debug("%s:%ld:%ld:%d", __FUNCTION__, t, timeReference, uncertainty);
    return 0;
}

static int inject_location(double latitude, double longitude, float accuracy)
{
    log_debug("%s", __FUNCTION__);
    return 0;
}

static void delete_aiding_data(GpsAidingData flags)
{
    log_debug("%s", __FUNCTION__);
}

static int set_position_mode(
    GpsPositionMode mode,
    GpsPositionRecurrence recurrence,
    uint32_t min_interval,
    uint32_t preferred_accuracy,
    uint32_t preferred_time)
{
    log_debug("%s", __FUNCTION__);
    return 0;
}

static const void* get_extension(const char* name)
{
    log_debug("%s:%s", __FUNCTION__, name);
    return 0;
}

const GpsInterface* gps_get_hardware_interface()
{
    log_debug("%s", __FUNCTION__);
    return (GpsInterface*)&dev;
}

static int fastrax_gps_open(const struct hw_module_t* module,
                            char const* name,
                            struct hw_device_t** device)
{
    log_debug("%s", __FUNCTION__);
    struct gps_device_t* dev = calloc(1, sizeof(struct gps_device_t));
    memset(dev, 0, sizeof(*dev));
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*)module;
    dev->get_gps_interface = gps_get_hardware_interface;

    *device = (struct hw_device_t*)dev;
    return 0;
}

static struct hw_module_methods_t gps_methods = {
    .open = fastrax_gps_open
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
    tag : HARDWARE_MODULE_TAG,
    version_major : 1,
    version_minor : 0,
    id : GPS_HARDWARE_MODULE_ID,
    name : "Fastrax GPS reader",
    author : "Boundary Devices",
    methods : &gps_methods
};
