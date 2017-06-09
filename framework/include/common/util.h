#ifndef __COMMON_UTIL_H__
#define __COMMON_UTIL_H__

#include <common/def.h>

// #ifndef __FILENAME__
// #define __FILENAME__            __FILE__
// #endif
//
// #define GW_LOG_HEADER_LEN       64
// #define GW_LOG_CONTENT_LEN      512
// #define GW_LOG_USE_PID          1
//
// #if GW_LOG_USE_PID
//     #define log_debug(...) \
//         do { \
//             char _h[GW_LOG_HEADER_LEN ] = {0}; \
//             char _c[GW_LOG_CONTENT_LEN] = {0}; \
//             snprintf(_h, GW_LOG_HEADER_LEN,  "[%-10ld][%-12.12s:%-3d][%-4d] ", \
//                     (long)time(NULL), __FILENAME__, __LINE__, getpid()); \
//             snprintf(_c, GW_LOG_CONTENT_LEN, ##__VA_ARGS__); \
//             printf("%s%s\n", _h, _c); \
//         } while(0)
// #else
//     #define log_debug(...) \
//         do { \
//             char _h[GW_LOG_HEADER_LEN ] = {0}; \
//             char _c[GW_LOG_CONTENT_LEN] = {0}; \
//             snprintf(_h, GW_LOG_HEADER_LEN,  "[%-12.12s:%-3d][%-10u] ", \
//                     __FILENAME__, __LINE__, (unsigned int)pthread_self()); \
//             snprintf(_c, GW_LOG_CONTENT_LEN, ##__VA_ARGS__); \
//             printf("%s%s\n", _h, _c); \
//         } while(0)
// #endif
//
// #define logger__                log_debug("----------");
// #define logger_addr(_p)         log_debug("ADDR: `" #_p "` addr is %p", _p);


// void dump(const unsigned char *bin, int len);
void thread_sleep(uint32_t millisec);

#endif//__COMMON_UTIL_H__
