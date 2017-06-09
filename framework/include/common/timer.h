/**
 * process only
 */
 #ifndef __FRAMEWORK_TIMER_H__
 #define __FRAMEWORK_TIMER_H__

#include <common/list.h>

typedef void (*timer_handler)(void *);

typedef struct {
    time_t expire_time;
    timer_handler handler;
    void *param;
} timer_node;


void timer_process(int sig);
void timer_init();
void timer_add(int delay, timer_handler handler, void *param);

#endif//__FRAMEWORK_TIMER_H__
