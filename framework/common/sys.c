#include <sys/shm.h>
#include <common/sys.h>
#include <common/log.h>

static env_t *_env = NULL;

env_t *env()
{
    if (!_env) {
        int shmid = shmget(SYS_ENV_SHM, sizeof(env_t), 0);
        if (shmid == -1) {
            log_debug("shmget failed. err: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }

        _env = (env_t *)shmat(shmid, 0, 0);
        if (_env == (void*)-1) {
            log_debug("shmat failed. err: %s", strerror(errno));
            exit(EXIT_FAILURE);
        }
        log_debug("shm attached at %p", _env);
    }

    return _env;
}

void reg_pid(pid_t pid)
{
    if (!env()) return;
    log_debug("reg pid: %ld", pid);
    env()->pidv[env()->pids++] = pid;
}
