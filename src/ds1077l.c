#include "ds1077l.h"
#include "ds1077l-bus.h"
#include "ds1077l-div.h"

#include <argp.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <fcntl.h>

const struct argp_option common_options[] = {
    {
        .name  = "address",
        .key   = 'a',
        .arg   = "0x5[8-f]",
        .flags = 0,
        .doc   = "The current address of the timer, between 0x58 and 0x5f. "
                 "Defaults to 0x58.",
        .group = 0
    },
    {
        .name  = "bus-dev",
        .key   = 'd',
        .arg   = I2C_BUS_DEVICE,
        .flags = 0,
        .doc   = "Path to the i2c bus the oscillator is attached to.",
        .group = 0
    },
    {
        .name  = "verbose",
        .key   = 'v',
        .arg   = 0,
        .flags = 0,
        .doc   = "Produce verbose output.",
        .group = 0
    },
    {0}
};

const struct argp common_argp = {
    common_options,
    parse_common_opts,
    NULL,
    "Comon arguments for utilities communicating with the Maxim DS1077L."
};

error_t
parse_common_opts (int key, char *arg, struct argp_state *state)
{
    ds1077l_common_args_t *args = state->input;

    switch (key) {
    case 'a':
        /* address is unsigned 8bit integer between 0x58 and 0x5f */
        args->address = strtol (arg, NULL, 16);
        if (args->address < 0x58 | args->address > 0x5f)
            argp_usage (state);
        break;
    case 'd':
        /* path to bus device node is validated in the main program */
        args->bus_dev = arg;
        break;
    case 'v':
        args->verbose = true;
        break;
    case ARGP_KEY_INIT:
        args->address = DS1077L_ADDR_DEFAULT;
        args->bus_dev = I2C_BUS_DEVICE;
        args->verbose = false;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

/* Convenience function to get a file descriptor for an i2c bus. Set the device
   address (second parameter) to 0 and the default address for the DS1077L will
   be used.
 */
int handle_get(char* dev_node, uint8_t dev_addr)
{
    int fd = 0;
    uint8_t addr = 0;

    if (dev_addr > 0)
        addr = dev_addr;
    else
        addr = DS1077L_ADDR_DEFAULT;

    fd = open(dev_node, O_RDWR);
    if (fd == -1)
        return -1;
    if (ioctl(fd, I2C_SLAVE, addr))
        return -1;
    return fd;
}
