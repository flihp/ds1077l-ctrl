#include "ds1077l.h"
#include "ds1077l-dump.h"

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEV "/dev/i2c-1"
#define USAGE "usage: %s /dev/i2c-[0-9]\n"

static error_t parse_opts (int key, char *arg, struct argp_state *state);

/* arguments:
 *   bus-dev: device node for bus
 *   address: device address
 */
const struct argp_option options[] = {
    {
        .name  = "bus-dev",
        .key   = 'd',
        .arg   = "/dev/i2c-[0-9]+",
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "Path to the device representing the i2c bus the oscillator "
                 "is attached to.",
        .group = 0
    },
    {
        .name  = "address",
        .key   = 'a',
        .arg   = "0x58-5f",
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "The address of the socilator.",
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

const struct argp argps = {
    options,
    parse_opts,
    NULL,
    "Dump registers from a DS1077L oscillator."
};

static error_t
parse_opts (int key, char *arg, struct argp_state *state)
{
    dump_args_t *dump_args = state->input;

    switch (key) {
        case 'd':
            dump_args->bus_dev = arg;
            break;
        case 'a':
            dump_args->address = strtol (arg, NULL, 16);
            if (dump_args->address < 0x58 || dump_args->address > 0x5f)
                argp_usage (state);
            break;
        case 'v':
            dump_args->verbose = true;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static void
dump_args_dump (dump_args_t *dump_args)
{
    printf ("User provided options:\n");
    printf ("  bus-dev: %s\n", dump_args->bus_dev);
    printf ("  address: 0x%x\n", dump_args->address);
    printf ("  verbose: %s\n", dump_args->verbose ? "true" : "false");
}

int main(int argc, char* argv[])
{
    ds1077l_bus_t bus = {0};
    ds1077l_div_t div = {0};
    ds1077l_mux_t mux = {0};
    int fd = 0;
    char* dev_path = NULL;
    dump_args_t dump_args = {
        .address = DS1077L_ADDR_DEFAULT,
        .bus_dev = I2C_BUS_DEVICE,
        .verbose = false,
    };

    if (argp_parse (&argps, argc, argv, 0, NULL, &dump_args)) {
        perror ("argp_parse: \n");
        exit (1);
    }
    if (dump_args.verbose == true)
        dump_args_dump (&dump_args);
    fd = handle_get (dump_args.bus_dev, dump_args.address);
    if (fd == -1) {
        perror("handle_get: ");
        exit(1);
    }
    printf ("Current state of device 0x%x on bus %s:\n",
             dump_args.address, dump_args.bus_dev);
    if (bus_get(fd, &bus) == -1) {
        perror("bus_get: ");
        exit(1);
    }
    bus_pretty(&bus);
    if (mux_get(fd, &mux) == -1) {
        perror("mux_get: ");
        exit(1);
    }
    mux_pretty(&mux);
    if (div_get(fd, &div) == -1) {
        perror("div_get: ");
        exit(1);
    }
    div_pretty(&div);
    exit(0);
}
