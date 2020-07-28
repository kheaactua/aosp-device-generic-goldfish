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

int fp = 0;

int opersyshw__read(char* buffer, int length)
{
    D("OPERSYS HW - read()for %d bytes called", length);

    return read(fp, buffer, length);
}

int opersyshw__write(char* buffer, int length)
{
    D("OPERSYS HW - write()for %d bytes called", length);

    return write(fp, buffer, length);
}

int opersyshw__test(int value)
{
    return value;
}

static int open_opersyshw(const struct hw_module_t* module, char const* name,
        struct hw_device_t** device)
{
    opersyshw_device_t *dev = malloc(sizeof(opersyshw_device_t));
    memset(dev, 0, sizeof(*dev));

    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = (struct hw_module_t*)module;
    dev->read = opersyshw__read;
    dev->write = opersyshw__write;
    dev->test = opersyshw__test;

    *device = (struct hw_device_t*) dev;

    fp = open("/dev/circchar", O_RDWR);

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
