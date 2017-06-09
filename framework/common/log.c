#include <common/log.h>
#include <common/sys.h>
#include <common/str.h>

// #define MAX_BUFFER_SIZE     10240
// #define LOG_FILE_DIR        "./log"

static pid_t pid;
static int fd;
static char *buffer;

static bool is_path_exist(const char *path)
{
    if (path == NULL)
        return false;
    if (access(path, F_OK) == 0)
        return true;
    return false;
}

void log_init(char *name)
{
    if (name == NULL)
        name = "common";

    if (is_path_exist(env()->sys.log_path) == false) {
        if (mkdir(env()->sys.log_path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == -1)
            exit(false);
    }

    str_t path = str_new__len(1024);
    str_append(path, env()->sys.log_path, "/", name, ".log");

    if (is_path_exist(path) == true) {
        for (int i = 2; ; i++) {
            str_t newpath = str_new__fmt(1024, "%s.%d", path, i);
            if (is_path_exist(newpath) == false) {
                rename(path, newpath);
                str_release(newpath);
                break;
            }
        }
    }

    fd = open(path, O_RDWR | O_CREAT, 0644);
    str_release(path);
}

void log_write(char *str)
{
    if (fd > 0) {
        write(fd, str, strlen(str));
    }
#if USE_CONSOLE_LOG
    else {
        printf(str);
    }
#endif
}

void log_fmt(char *level, char *filename, int line, char *fmt, ...)
{
    static char head[64]        = {0};
    static char body[1024]      = {0};
    static char content[1100]   = {0};

    snprintf(head, 64,  "[%s][%-10ld][%-12.12s:%-3d][%-4d] ",
        level, (long)time(NULL), filename, line, getpid());

    va_list argv;
    va_start(argv, fmt);
    vsnprintf(body, 1024, fmt, argv);
    va_end(argv);

    snprintf(content, 1100,  "%s%s\n", head, body);

    log_write(content);
}

void dump(const unsigned char* bin, int len)
{
    static char buffer[1100] = {0};
    int size = 0;
    int line = 0;
    int coln = 0;
    size += snprintf(buffer + size, 1100 - size,  "dump:\n");
    size += snprintf(buffer + size, 1100 - size,  "%4d:", line++);
    for (int i = 0; i < len; i++) {
        size += snprintf(buffer + size, 1100 - size,  " 0x%02X", *(bin + i));
        coln++;
        if (coln == 4) {
            printf(" ");
            size += snprintf(buffer + size, 1100 - size,  " ");
        } else if (coln == 8) {
            size += snprintf(buffer + size, 1100 - size,  "\n%4d:", line++);
            coln = 0;
        }
    }
    snprintf(buffer + size, 1100 - size,  "\n");
    log_write(buffer);
}
