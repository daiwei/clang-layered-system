#ifndef __COMMON_LOG_H__
#define __COMMON_LOG_H__

#include <common/def.h>


#define USE_CONSOLE_LOG     1


#ifndef __FILENAME__
#   define __FILENAME__     __FILE__
#endif


void log_init(char *name);
void log_fmt(char *level, char *filename, int line, char *fmt, ...);

#define log_info(...)       log_fmt("INFO ", __FILENAME__, __LINE__, __VA_ARGS__)
#define log_debug(...)      log_fmt("DEBUG", __FILENAME__, __LINE__, __VA_ARGS__)
#define log_error(...)      log_fmt("ERROR", __FILENAME__, __LINE__, __VA_ARGS__)

void dump(const unsigned char* bin, int len);

#endif//__COMMON_LOG_H__
