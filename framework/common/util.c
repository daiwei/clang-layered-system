#include <common/util.h>
#include <common/log.h>

// void dump(const unsigned char* bin, int len)
// {
//     log_debug("dump:");
//     int line = 0;
//     int coln = 0;
//     printf("%4d:", line++);
//     for (int i = 0; i < len; i++) {
//         printf(" 0x%02X", *(bin + i));
//         coln++;
//         if (coln == 4) {
//             printf(" ");
//         } else if (coln == 8) {
//             printf("\n%4d:", line++);
//             coln = 0;
//         }
//     }
//     printf("\n");
// }

void thread_sleep(uint32_t millisec)
{
    struct timeval tv;
    tv.tv_sec = millisec / 1000;
    tv.tv_usec = millisec % 1000;
    select(0, 0, 0, 0, &tv);
}
