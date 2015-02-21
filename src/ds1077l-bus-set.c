#include "ds1077l.h"
#include "ds1077l-bus-set.h"

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void bus_args_dump (bus_args_t *bus_args);
static error_t parse_opts (int key, char *arg, struct argp_state *state);

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
        .name  = "new-addr",
        .key   = 'n',
        .arg   = "hex-num",
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "The new address to assign to the timer specified by the "
                 "\'address\' parameter",
        .group = 0
    },
    {
        .name  = "wc",
        .key   = 'w',
        .arg   = "flag",
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "When set to 0 all changes to registers will be written to "
                 "EEPROM. When set to 1, an explicit WRITE command is "
                 "required.",
        .group = 0
    },
    {
        .name  = "bus",
        .key   = 'b',
        .arg   = "bus-device",
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "Path to the i2c bus the timer is attached to.",
        .group = 0
    },
    { 0 }
};

const struct argp argps = {
    options,
    parse_opts,
    NULL,
    "Set values in the BUS register of a Maxim DS1077L programmable oscillator."
};

static error_t
parse_opts (int key, char *arg, struct argp_state *state)
{
    bus_args_t* bus_args = state->input;
    uint32_t number = 0;

    switch (key)
    {
        case 'a':
            /* address is unsigned 8bit integer between 0x58 and 0x5f */
            number = strtol (arg, NULL, 16);
            if (number < 0x58 | number > 0x5f)
                argp_usage (state);
            bus_args->address = number;
            break;
        case 'b':
            /* path to bus device node is validated in the main program */
            bus_args->bus = arg;
            break;
        case 'n':
            /* same as above */
            number = strtol (arg, NULL, 16);
            if (number < 0x58 | number > 0x5f)
                argp_usage (state);
            bus_args->new_addr = number;
            break;
        case 'w':
            /* wc is either 0 or 1 */
            if (strnlen (arg, 2) != 1)
                argp_usage (state);
            number = strtol (arg, NULL, 10);
            if (number < 0 | number > 1)
                argp_usage (state);
            bus_args->write_on_change = number;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static void
bus_args_dump (bus_args_t *bus_args)
{
    printf ("address:         0x%x\n", bus_args->address);
    printf ("new_addr:        0x%x\n", bus_args->new_addr);
    printf ("bus:             %s\n",   bus_args->bus);
    printf ("write_on_change: 0x%x\n", bus_args->write_on_change);
}

int
main (int argc, char *argv[])
{
    int fd = 0;
    /* argument structure populated with defaults */
    bus_args_t bus_args = {
        .address = 0x58,
        .new_addr = 0x58,
        .bus = "/dev/i2c-1",
        .write_on_change = 0        
    };
    if (argp_parse (&argps, argc, argv, 0, NULL, &bus_args)) {
        perror ("argp_parse: \n");
        exit (1);
    }
    printf("parsed\n");
    exit (0);
}

