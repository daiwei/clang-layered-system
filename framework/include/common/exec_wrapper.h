#include <common/module.h>

#define EXEC_WRAPPER(_name, ...)                                                                    \
                                                                                                    \
    MODULE_SERVICE(_name);                                                                          \
                                                                                                    \
    static pid_t exec_pid = 0;                                                                      \
                                                                                                    \
    static void wait_exec(pid_t pid)                                                                \
    {                                                                                               \
        pid_t epid = waitpid(pid, NULL, 0);                                                         \
        log_debug("service %d exit", epid);                                                            \
        this->status = STOP;                                                                        \
    }                                                                                               \
                                                                                                    \
    static void kill_exec(pid_t pid)                                                                \
    {                                                                                               \
        kill(pid, SIGKILL);                                                                         \
        this->status = STOP;                                                                        \
        sleep(1);                                                                                   \
    }                                                                                               \
                                                                                                    \
    static int start()                                                                              \
    {                                                                                               \
        if (exec_pid > 0 && this->status == RUNNING) {                                              \
            kill_exec(exec_pid);                                                                    \
        }                                                                                           \
                                                                                                    \
        pid_t pid = fork();                                                                         \
        if (pid == 0) {                                                                             \
            log_debug("child  processor(%d)", getpid());                                               \
            execlp(_name, _name, ##__VA_ARGS__, NULL);                                              \
        } else if(pid > 0) {                                                                        \
            exec_pid = pid;                                                                         \
            wait_exec(exec_pid);                                                                    \
            return 1;                                                                               \
        } else {                                                                                    \
            log_debug("service %s start error", this->info->module_id);                                \
            return 0;                                                                               \
        }                                                                                           \
                                                                                                    \
        return 0;                                                                                   \
    }                                                                                               \
                                                                                                    \
    static int destroy()                                                                            \
    {                                                                                               \
        kill_exec(exec_pid);                                                                        \
        return 0;                                                                                   \
    }                                                                                               \
                                                                                                    \
    static int on_load()                                                                            \
    {                                                                                               \
        return 0;                                                                                   \
    }                                                                                               \
                                                                                                    \
    static int on_unload()                                                                          \
    {                                                                                               \
        return 0;                                                                                   \
    }
