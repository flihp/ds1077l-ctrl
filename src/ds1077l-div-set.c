#include "ds1077l-div-set.h"

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void div_args_dump (div_args_t *div_args);
static error_t parse_opts (int key, char *arg, struct argp_state *state);

/* arguments:
 * bus-dev: device node for bus
 * address: device address
 * divider: N divider on OUT1
 */
const struct argp_option options[] = {
    {
        .name  = "address",
        .key   = 'a',
        .arg   = "hex-num",
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "The current address of the timer, between 0x58 and 0x5f. "
                 "Defaults to 0x58.",
        .group = 0
    },
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
        .name  = "divider",
        .key   = 'n',
        .arg   = "2 - 1025",
        .flags = 0,
        .doc   = "Value of the programmable divider on OUT1.",
        .group = 0
    },
    { 0 }
};

const struct argp argps = {
    options,
    parse_opts,
    NULL,
    "Set values in the DIV register of a Maxim DS1077L programmable oscillator."
};

static error_t
parse_opts (int key, char *arg, struct argp_state *state)
{
    div_args_t* div_args = state->input;
    uint32_t number = 0;

    switch (key) {
        case 'a':
            /* address is unsigned 8bit integer between 0x58 and 0x5f */
            div_args->address = strtol (arg, NULL, 16);
            if (div_args->address < 0x58 | div_args->address > 0x5f)
                argp_usage (state);
            break;
        case 'b':
            /* path to bus device node is validated in the main program */
            div_args->bus_dev = arg;
            break;
        case 'n':
            /* n divider value, between 2 and 1025 decimal */
            div_args->divider = strtol (arg, NULL, 10);
            if (div_args->divider < 0x2 | div_args->divider > 0x401)
                argp_usage (state);
            div_args->divider_set = true;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static void
div_args_dump (div_args_t *div_args)
{
    printf ("address: 0x%x\n", div_args->address);
    printf ("bus-dev: %s\n",   div_args->bus_dev);
    printf ("divider: %d\n",   div_args->divider);
}

int
main (int argc, char *argv[])
{
    int fd = 0;
    /* argument structure populated with defaults */
    div_args_t div_args = {
        .address = DS1077L_ADDR_DEFAULT,
        .bus_dev = I2C_BUS_DEVICE,
        .divider = DS1077L_N_DEFAULT,
        .divider_set = false,
    };
    ds1077l_div_t div = {0};

    if (argp_parse (&argps, argc, argv, 0, NULL, &div_args)) {
        perror ("argp_parse: \n");
        exit (1);
    }
    if (!div_args.divider_set) {
        argp_help (&argps, stderr, ARGP_HELP_USAGE, NULL);
        exit (1);
    }
    printf ("");
    div_args_dump (&div_args);
    fd = handle_get (div_args.bus_dev, div_args.address);
    if (fd == -1) {
        perror ("handle_get: ");
        exit (1);
    }
    /* get current register state and display to user */
    if (div_get (fd, &div)) {
        perror ("div_set: ");
        exit (1);
    }
    printf ("Current DIV register state:\n");
    div_pretty (&div);
    /* populate new structure, display to user, and make change
    if (bus_args.new_addr_set)
        bus.address = bus_args.new_addr;
    if (bus_args.write_on_change_set)
        bus.wc = bus_args.write_on_change;
    printf ("Setting device 0x%x on bus %s to:\n",
            bus_args.address, bus_args.bus);
    bus_pretty (&bus);
    if (bus_set (fd, &bus)) {
        perror ("bus_set: ");
        exit (1);
    }
    */
    exit (1);
}

