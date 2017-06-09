/**
 **********************************************************
 * INSTALLER README
 **********************************************************
 *
 * auth:
 *   app will install by `root`, and run as `user`
 *
 * app package format:
 *   zip + md5(32byte) + type(1byte)
 *
 * app necessary files:
 *   500  app.icon  -- launch icon
 *   500  app.so    -- launch entry, loaded by app_loader
 *   500  lib
 *   500  resource
 *   700  user-data -- user controled data
 *
 * pre-install package path:
 *   /path/to/package
 *
 * installed app path:
 *   /path/to/app
 *
 * e.g. app `demo`:
 *   package path: /path/to/package/demo.zip
 *   app path:     /path/to/app/demo
 *
 *
 * 打包:
 * 1) app
 *      a) new app
 *      b) zip app
 *      c) add md5sum & type
 * 2) sys
 *      a) zip build
 *      c) add md5sum & type
 *
 * 安装升级:
 * 1) app
 * 2) system
 *
 *
 */
#include <zip/zip.h>
#include <common/str.h>
#include <common/loader.h>
#include <service/installer.h>
#include <dirent.h>

// =========================================================
// user-defined: data definition & function declaration
// =========================================================
int sniff_new();
int upgrade_sys();
int install_app(str_t filename);
int verify(str_t filename);
int stop_app(str_t filename);
int unzip_and_install(str_t filename);
int check(str_t unzip_path);


// =========================================================
// module definition, will define `this` points to module
// =========================================================
MODULE_SERVICE("installer");
// EXPORT(tracker_interface_t);


// =========================================================
// module rpc method
// =========================================================


// =========================================================
// module state action
// =========================================================
static int start()
{
    log_debug("enter function %s", __FUNCTION__);

    while (1) {
        sniff_new();
        sleep(1);
    }

    return 0;
}


// =========================================================
// module load & unload method
// =========================================================
static int on_load()
{
    log_debug("enter function %s", __FUNCTION__);
    // -----------------------------------------------------
    // register module's rpc interface
    // -----------------------------------------------------


    // -----------------------------------------------------
    // import module's dependent interface
    // -----------------------------------------------------


    // -----------------------------------------------------
    // init process after module loaded
    // -----------------------------------------------------
    this->start = start;


    return 0;
}

static int on_unload()
{
    return 0;
}

// =========================================================
// user-defined: function definition
// =========================================================
int sniff_new()
{
    DIR *dir;
    struct dirent *ptr;
    // str_t pkg_path = str_new__cstr(PKG_PATH);

    dir = opendir(PKG_PATH);
    while ((ptr = readdir(dir)) != NULL) {
        // log_debug("d_type:%d d_name: %s", ptr->d_type, ptr->d_name);
        str_t filename = str_new__cstr(ptr->d_name);
        if (str_equal(filename, "sys.img")) {
            log_debug("upgrade system %s", ptr->d_name);
            upgrade_sys();
        }

        if (str_end_with(filename, ".app")) {
            log_debug("found new app to install %s", ptr->d_name);
            install_app(filename);
        }
    }
    closedir(dir);
    return 1;
}

int upgrade_sys()
{
    rename("download/sys.img", "download/sys.img.lock");

    // pid_t pid = fork();
    // if (pid == 0) {         /* child processor */
    //     daemon(0, 0);
    //     system("./init/upgrade.sh");
    // } else if (pid > 0) {   /* parent processor */
    //     return 1;
    // } else {
    //     log_debug("fork error");
    //     return 0;
    // }

    // system("./init/stop.sh");
    system("nohup ./init/upgrade.sh &");
    // system("nohup sh ./init/upgrade.sh &>/dev/null &");
    // system("./init/start.sh");
}

int install_app(str_t filename)
{
    if (!verify(filename)) return 0;
    unzip_and_install(filename);
}

int verify(str_t filename)
{
    // int fd = open(filename);
    // struct stat fst;
    // // fstat(fd, &fst);
    // // fst.st_size
    // lseek(fd, 33, SEEK_END);
    //
    // app_mark_t mark;
    // read(fd, &mark, 33);

    return 1;
}

int stop_app(str_t filename)
{
    module_t *module = get_module(ID_APP, filename);
    msg_command(module, MSG_CMD_STOP);
    unload(module);
}

int on_extract_entry(const char *filename, void *arg)
{
    log_debug("extract %s", filename);
    return 1;
}

int unzip_and_install(str_t filename)
{
    str_t pkg_path = str_new__cstr(PKG_PATH);
    pkg_path = str_append(pkg_path, "/", filename);

    str_t unzip_path = str_new__cstr(UNZIP_PATH);
    unzip_path = str_append(unzip_path, "/", filename, ".tmp");

    zip_extract(pkg_path, unzip_path, on_extract_entry, NULL);
    remove(pkg_path);

    check(unzip_path);

    // install app
    str_t app_path = str_new__cstr(APP_PATH);
    str_t *split_name = str_split(filename, ".");
    app_path = str_append(app_path, "/", split_name[0]);
    rename(unzip_path, app_path);

    return 1;
}


int check(str_t unzip_path)
{
    log_debug("enter function %s", __FUNCTION__);
    //necessary files:
    // app.icon
    // app.so
    // lib
    // resource
    // user-data
    return 0;
}

static int access_mode()
{
    log_debug("enter function %s", __FUNCTION__);
    //mode: 500
    //user-data: 700
    return 0;
}

static int reg_app()
{
    log_debug("enter function %s", __FUNCTION__);
    //register app
    //  icon for desktop launcher
    return 0;
}
