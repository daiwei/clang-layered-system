#ifndef __COMMON_RINGBUF_H__
#define __COMMON_RINGBUF_H__

#include <common/def.h>

#define MAX_RINGBUF_SIZE     1024

typedef struct {
    byte buffer[MAX_RINGBUF_SIZE];
    int size;
    byte *r_cursor;
    byte *w_cursor;
} ringbuf_t;

ringbuf_t *ringbuf_new();
int ringbuf_size(ringbuf_t *ringbuf);
void ringbuf_reset(ringbuf_t *ringbuf);
int ringbuf_find(ringbuf_t *ringbuf, char separator);
int ringbuf_read(ringbuf_t *ringbuf, byte *buf, int size);
int ringbuf_write(ringbuf_t *ringbuf, byte *buf, int size);

#endif//__COMMON_RINGBUF_H__
