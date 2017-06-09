#include <common/timer.h>

static list_t *timer_handlers;


void timer_process(int sig)
{
    if (!timer_handlers || !list_size(timer_handlers)) {
        return;
    }

    time_t current_time = time(NULL);

    list_iter_t *iter = list_iter(timer_handlers);
    while (list_iter_has_next(iter)) {
        list_node_t *node = list_iter_next(iter);
        timer_node *tn = node->data;
        if (current_time >= tn->expire_time) {
            tn->handler(tn->param);
            list_iter_remove(iter);
        } else {
            break;
        }
    }
    list_iter_free(iter);

}

void timer_init()
{
    timer_handlers = list_new();
    signal(SIGALRM, timer_process);
}

void timer_add(int delay, timer_handler handler, void *param)
{
    time_t expire_time = delay + time(NULL);

    list_iter_t *iter = list_iter(timer_handlers);
    while (list_iter_has_next(iter)) {
        list_node_t *node = list_iter_next(iter);
        timer_node *tn = node->data;
        if (expire_time < tn->expire_time) {
            timer_node *add = calloc(1, sizeof(timer_node));
            add->expire_time = expire_time;
            add->handler = handler;
            add->param = param;
            list_iter_prepend(iter, add);
        }
    }
    list_iter_free(iter);

    alarm(delay);
}
