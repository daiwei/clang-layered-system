#include <common/ringbuf.h>
#include <common/log.h>

ringbuf_t *ringbuf_new()
{
    ringbuf_t *ringbuf = calloc(1, sizeof(ringbuf_t));
    ringbuf->w_cursor = ringbuf->buffer;
    ringbuf->r_cursor = ringbuf->buffer;
    return ringbuf;
}

int ringbuf_size(ringbuf_t *ringbuf)
{
    if (!ringbuf) {
        log_error("arg is null");
        return -1;
    }
    return ringbuf->size;
}

void ringbuf_reset(ringbuf_t *ringbuf)
{
    if (!ringbuf) {
        log_error("arg is null");
        return;
    }
    memset(ringbuf, 0, sizeof(ringbuf_t));
}

static byte *ringbuf_r_offset(ringbuf_t *ringbuf, int offset)
{
    if (!ringbuf) {
        log_error("arg is null");
        return NULL;
    }

    if (offset < 0) {
        return NULL;
    }

    if (offset >= MAX_RINGBUF_SIZE) {
        offset -= MAX_RINGBUF_SIZE;
    }

    if (offset > ringbuf->size) {
        offset = ringbuf->size;
    }

    if (ringbuf->r_cursor + offset < ringbuf->buffer + MAX_RINGBUF_SIZE) {
        return ringbuf->r_cursor + offset;
    } else {
        int idx = ringbuf->r_cursor + offset - (ringbuf->buffer + MAX_RINGBUF_SIZE);
        return ringbuf->buffer + idx;
    }
}

static byte *ringbuf_w_offset(ringbuf_t *ringbuf, int offset)
{
    if (!ringbuf) {
        log_error("arg is null");
        return NULL;
    }

    if (offset < 0) {
        return NULL;
    }

    if (offset >= MAX_RINGBUF_SIZE) {
        offset -= MAX_RINGBUF_SIZE;
    }

    if (ringbuf->w_cursor + offset < ringbuf->buffer + MAX_RINGBUF_SIZE) {
        return ringbuf->w_cursor + offset;
    } else {
        int idx = ringbuf->w_cursor + offset - (ringbuf->buffer + MAX_RINGBUF_SIZE);
        return ringbuf->buffer + idx;
    }
}

int ringbuf_find(ringbuf_t *ringbuf, char separator)
{
    if (!ringbuf) {
        log_error("arg is null");
        return -1;
    }

    for (int i = 0; i < ringbuf->size; i++) {
        if (*ringbuf_r_offset(ringbuf, i) == separator) {
            return i;
        }
    }

    return -1;
}

int ringbuf_read(ringbuf_t *ringbuf, byte *buf, int size)
{
    if (!ringbuf || !buf) {
        log_error("arg is null");
        return -1;
    }
    // log_debug("ringbuf_write: arg size:     %d", size);
    // log_debug("ringbuf_write: rb.size:      %d", ringbuf->size);
    // log_debug("ringbuf_write: rb.buffer:    %p", ringbuf->buffer);
    // log_debug("ringbuf_write: rb.r_cursor:  %p", ringbuf->r_cursor);
    // log_debug("ringbuf_write: rb.w_cursor:  %p", ringbuf->w_cursor);

    if (size <= 0) {
        return 0;
    }
    if (size > MAX_RINGBUF_SIZE - ringbuf->size) {
        size = MAX_RINGBUF_SIZE - ringbuf->size;
    }
    if (size > ringbuf->size) {
        size = ringbuf->size;
    }

    if (ringbuf->r_cursor + size > ringbuf->buffer + MAX_RINGBUF_SIZE) {
        int part1 = ringbuf->buffer + MAX_RINGBUF_SIZE - ringbuf->r_cursor;
        memcpy(buf, ringbuf->r_cursor, part1);
        memcpy(buf + part1, ringbuf->buffer, size - part1);
    } else {
        memcpy(buf, ringbuf->r_cursor, size);
    }

    ringbuf->r_cursor = ringbuf_r_offset(ringbuf, size);
    ringbuf->size -= size;
    // log_debug("ringbuf_write: rb.size:      %d", ringbuf->size);
    // log_debug("ringbuf_write: rb.buffer:    %p", ringbuf->buffer);
    // log_debug("ringbuf_write: rb.r_cursor:  %p", ringbuf->r_cursor);
    // log_debug("ringbuf_write: rb.w_cursor:  %p", ringbuf->w_cursor);

    return size;
}

int ringbuf_write(ringbuf_t *ringbuf, byte *buf, int size)
{
    if (!ringbuf || !buf) {
        log_error("arg is null");
        return -1;
    }
    // log_debug("ringbuf_write: arg size:     %d", size);
    // log_debug("ringbuf_write: rb.size:      %d", ringbuf->size);
    // log_debug("ringbuf_write: rb.buffer:    %p", ringbuf->buffer);
    // log_debug("ringbuf_write: rb.r_cursor:  %p", ringbuf->r_cursor);
    // log_debug("ringbuf_write: rb.w_cursor:  %p", ringbuf->w_cursor);

    if (size <= 0) {
        return 0;
    }

    if (size >= MAX_RINGBUF_SIZE) {
        memcpy(ringbuf->buffer, buf + size - MAX_RINGBUF_SIZE, MAX_RINGBUF_SIZE);
        ringbuf->size = MAX_RINGBUF_SIZE;
        ringbuf->w_cursor = ringbuf->buffer;
        ringbuf->r_cursor = 0;

        size = MAX_RINGBUF_SIZE;
    } else {
        if (ringbuf->size + size > MAX_RINGBUF_SIZE) {
            size = MAX_RINGBUF_SIZE - ringbuf->size;
        }

        if (ringbuf->w_cursor + size <= ringbuf->buffer + MAX_RINGBUF_SIZE) {
            memcpy(ringbuf->w_cursor, buf, size);
        } else {
            int part1 = ringbuf->buffer + MAX_RINGBUF_SIZE - ringbuf->w_cursor;
            memcpy(ringbuf->w_cursor, buf,         part1);
            memcpy(ringbuf->buffer,   buf + part1, size - part1);
        }

        ringbuf->w_cursor = ringbuf_w_offset(ringbuf, size);
        ringbuf->size += size;
    }
    // log_debug("ringbuf_write: rb.size:      %d", ringbuf->size);
    // log_debug("ringbuf_write: rb.buffer:    %p", ringbuf->buffer);
    // log_debug("ringbuf_write: rb.r_cursor:  %p", ringbuf->r_cursor);
    // log_debug("ringbuf_write: rb.w_cursor:  %p", ringbuf->w_cursor);

    return size;
}
