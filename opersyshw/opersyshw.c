#include <errno.h>

#define  LOG_TAG  "opersyshw_hal_module"
#include <log/log.h>
#include <cutils/sockets.h>
#include <hardware/opersyshw.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define   OPERSYSHW_DEBUG   1

#if OPERSYSHW_DEBUG
#  define D(...)   ALOGD(__VA_ARGS__)
#else
#  define D(...)   ((void)0)
#endif

#define CMD_BASE                  0x12341
#define CMD_ZERO_CONTENT          CMD_BASE + 0
#define CMD_IS_THERE_CONTENT      CMD_BASE + 1

// Global file pointer
int fd = 0;

int opersyshw__read(char* buffer, int length)
{
    D("OPERSYS HW - read()for %d bytes called", length);

    int sz = read(fd, buffer, length);

    return sz;
}

int opersyshw__write(char* buffer, int length)
{
    D("OPERSYS HW - write()for %d bytes called", length);

    int sz = write(fd, buffer, length);

    return sz;
}

int opersyshw__test(int value)
{
    return value;
}

void opersyshw__zero_content()
{
    ioctl(fd, CMD_ZERO_CONTENT, 0);
}

int opersyshw__is_there_content()
{
    return ioctl(fd, CMD_IS_THERE_CONTENT);
}

static int open_opersyshw(const struct hw_module_t* module, char const* name,
        struct hw_device_t** device)
{
    opersyshw_device_t *dev = malloc(sizeof(opersyshw_device_t));
    memset(dev, 0, sizeof(*dev));

    dev->common.tag        = HARDWARE_DEVICE_TAG;
    dev->common.version    = 0;
    dev->common.module     = (struct hw_module_t*)module;
    dev->read              = opersyshw__read;
    dev->write             = opersyshw__write;
    dev->test              = opersyshw__test;
    dev->zero_content      = opersyshw__zero_content;
    devi->is_there_content = opersyshw__is_there_content;

    *device = (struct hw_device_t*) dev;

    // Global
    D("OPERSYS Opening /dev/circchar in rw");
    fd = open("/dev/circchar", O_RDWR);

    D("OPERSYS HW has been initialized");

    return 0;
}

static struct hw_module_methods_t opersyshw_module_methods = {
    .open = open_opersyshw
};

struct hw_module_t HAL_MODULE_INFO_SYM = {
    .tag = HARDWARE_MODULE_TAG,
    .version_major = 1,
    .version_minor = 0,
    .id = OPERSYSHW_HARDWARE_MODULE_ID,
    .name = "Opersys HW Module",
    .author = "Opersys inc.",
    .methods = &opersyshw_module_methods,
};
