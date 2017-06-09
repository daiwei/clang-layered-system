#include <common/ringbuf.h>
#include <criterion/criterion.h>

TestSuite(suite_ringbuf);

Test(suite_ringbuf, test_ringbuf_new) {
    ringbuf_t *rb = ringbuf_new();
    cr_assert_neq(NULL, rb);
}

Test(suite_ringbuf, test_ringbuf_write) {
    ringbuf_t *rb = ringbuf_new();
    cr_assert_eq(ringbuf_size(rb), 0);

    ringbuf_write(rb, "test", 4);
    cr_assert_eq(ringbuf_size(rb), 4);
    cr_assert_eq(rb->buffer + 4, rb->w_cursor);
    cr_assert_eq(rb->buffer, rb->r_cursor);

    ringbuf_write(rb, "test", 4);
    cr_assert_eq(ringbuf_size(rb), 8);
    cr_assert_eq(rb->buffer + 8, rb->w_cursor);
    cr_assert_eq(rb->buffer, rb->r_cursor);
}

Test(suite_ringbuf, test_ringbuf_read) {
    ringbuf_t *rb = ringbuf_new();
    cr_assert_eq(ringbuf_size(rb), 0);

    byte buf[100] = {0};

    ringbuf_write(rb, "test", 4);
    cr_assert_eq(ringbuf_size(rb), 4);
    cr_assert_eq(rb->buffer + 4, rb->w_cursor);
    cr_assert_eq(rb->buffer, rb->r_cursor);

    ringbuf_read(rb, buf, 2);
    cr_assert_eq(ringbuf_size(rb), 2);
    cr_assert_eq(rb->buffer + 4, rb->w_cursor);
    cr_assert_eq(rb->buffer + 2, rb->r_cursor);
}

Test(suite_ringbuf, test_ringbuf_find) {
    ringbuf_t *rb = ringbuf_new();
    cr_assert_eq(ringbuf_size(rb), 0);

    byte buf[100] = {0};

    ringbuf_write(rb, "test", 4);
    cr_assert_eq(ringbuf_size(rb), 4);
    cr_assert_eq(rb->buffer + 4, rb->w_cursor);
    cr_assert_eq(rb->buffer, rb->r_cursor);

    cr_assert_eq(ringbuf_find(rb, 's'), 2);
}
