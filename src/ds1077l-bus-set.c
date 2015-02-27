#include "ds1077l-bus.h"
#include "ds1077l-bus-set.h"

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void bus_args_dump (bus_args_t *bus_args);
static error_t parse_opts (int key, char *arg, struct argp_state *state);

const struct argp_option options[] = {
     {
        .name  = "new-addr",
        .key   = 'n',
        .arg   = "0x5[8-f]",
        .flags = 0,
        .flags = 0,
        .doc   = "The new address to assign to the timer specified by the "
                 "\'address\' parameter",
        .group = 1
    },
    {
        .name  = "wc",
        .key   = 'w',
        .arg   = "0|1",
        .flags = 0,
        .doc   = "When set to 0 all changes to registers will be written to "
                 "EEPROM. When set to 1, an explicit WRITE command is "
                 "required.",
        .group = 1
    },
    { 0 }
};

const struct argp_child argp_children[] = {
    {
        &common_argp,
        0,
        0,
        0
    },
    { 0 }
};

const struct argp argps = {
    options,
    parse_opts,
    NULL,
    "Set values in the BUS register of a Maxim DS1077L programmable oscillator.",
    argp_children
};

static error_t
parse_opts (int key, char *arg, struct argp_state *state)
{
    bus_args_t* bus_args = state->input;

    switch (key)
    {
        case 'n':
            /* same as above */
            bus_args->new_addr = strtol (arg, NULL, 16);
            if (bus_args->new_addr < 0x58 | bus_args->new_addr > 0x5f)
                argp_usage (state);
            bus_args->new_addr_set = true;
            break;
        case 'w':
            /* wc is either "true" or "false" */
            bus_args->wc = strtol (arg, NULL, 16);
            if (! (bus_args->wc == 0x0 || bus_args->wc == 0x1))
                argp_usage (state);
            bus_args->wc_set = true;
            break;
        case ARGP_KEY_INIT:
            bus_args->new_addr = DS1077L_ADDR_DEFAULT,
            bus_args->new_addr_set = false,
            bus_args->wc = DS1077L_WC_DEFAULT,
            bus_args->wc_set = false,
            state->child_inputs[0] = &(bus_args->common_args);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static void
bus_args_dump (bus_args_t *bus_args)
{
    printf ("User provided options:\n");
    printf ("  address:         0x%x\n", bus_args->common_args.address);
    printf ("  new_addr:        0x%x\n", bus_args->new_addr);
    printf ("  bus:             %s\n",   bus_args->common_args.bus_dev);
    printf ("  verbose:         %s\n",   bus_args->common_args.verbose ? "true" : "false");
    printf ("  wc:              %s\n",   bus_args->wc ? "true" : "false");
}

int
main (int argc, char *argv[])
{
    int fd = 0;
    /* argument structure populated with defaults */
    bus_args_t bus_args = {0};
    ds1077l_bus_t bus = {0};

    if (argp_parse (&argps, argc, argv, 0, NULL, &bus_args)) {
        perror ("argp_parse: \n");
        exit (1);
    }
    if (! (bus_args.new_addr_set | bus_args.wc_set)) {
        fprintf(stderr, "Either a new address or a new value for the wc bit must be provided.\n");
        exit (1);
    }
    if (bus_args.common_args.verbose)
        bus_args_dump (&bus_args);
    fd = handle_get (bus_args.common_args.bus_dev,
                     bus_args.common_args.address);
    if (fd == -1) {
        perror ("handle_get: ");
        exit (1);
    }
    /* get current register state and display to user */
    if (bus_get (fd, &bus)) {
        perror ("bus_set: ");
        exit (1);
    }
    if (bus_args.common_args.verbose) {
        printf ("Current BUS register state:\n");
        bus_pretty (&bus);
    }
    /* populate new structure, display to user, and make change */
    if (bus_args.new_addr_set)
        bus.address = bus_args.new_addr;
    if (bus_args.wc_set)
        bus.wc = bus_args.wc;
    if (bus_args.common_args.verbose) {
        printf ("Setting device 0x%x on bus %s to:\n",
                bus_args.common_args.address, bus_args.common_args.bus_dev);
        bus_pretty (&bus);
    }
    if (bus_set (fd, &bus)) {
        perror ("bus_set: ");
        exit (1);
    }
    exit (0);
}

