#include "ds1077l.h"
#include "ds1077l-writee2.h"

#include <linux/i2c-dev.h>
#include <stdlib.h>

static int
writee2 (int fd)
{
    /* This is the closest I could come to finding a way to write a command
     * byte with no data byte. The parameters are packed into a struct
     * named 'i2c_smbus_ioctl_data defined in i2c-dev.h as follows:
     * 'read_write': set to I2C_SMBUS_WRITE, 
     * 'command': set to the 'write E2' command from the DS1077L spec sheet
     * 'size': is set to 0
     * 'data': is a null pointer.
     */
    return i2c_smbus_access (fd, I2C_SMBUS_WRITE, COMMAND_E2_WRITE, 0, NULL);
}

int
main (int argc, char* argv[])
{
    int fd = 0;
    /* just the common arguments */
    ds1077l_common_args_t common_args = { 0 };

    if (argp_parse (&common_argp, argc, argv, 0, NULL, &common_args)) {
        perror ("argp_parse: \n");
        exit (1);
    }
    if (common_args.verbose)
        dump_common_opts (&common_args);
    fd = handle_get (common_args.bus_dev, common_args.address);
    if (fd == -1) {
        perror ("handle_get: ");
        exit (1);
    }
    if (writee2 (fd) != 0) {
        perror ("writee2: \n");
        exit (1);
    }
    if (common_args.verbose)
        printf ("writee2: success!\n");
    exit (0);
}
