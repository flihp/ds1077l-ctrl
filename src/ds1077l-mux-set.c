#include "ds1077l.h"
#include "ds1077l-mux-set.h"

#include <argp.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static void mux_args_dump (mux_args_t *mux_args);
static error_t parse_opts (int key, char *arg, struct argp_state *state);

/* arguments */
const struct argp_option options[] = {
    {
        .name  = "address",
        .key   = 'a',
        .arg   = "0x58-0x5f",
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "The address of the oscillator. Defaults to 0x58.",
        .group = 0
    },
    {
        .name  = "bus-dev",
        .key   = 'd',
        .arg   = I2C_BUS_DEVICE,
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "Path to the device representing the i2c bus the oscillator "
                 "is attached to.",
        .group = 0
    },
    {
        .name  = "pdn1",
        .key   = 'e',
        .arg   = "0|1",
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "Value to set for the PDN1 bit.",
        .group = 0
    },
    {
        .name  = "pdn0",
        .key   = 'f',
        .arg   = "0|1",
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "Value to set for the PDN0 bit.",
        .group = 0
    },
    {
        .name  = "sel0",
        .key   = 'g',
        .arg   = "0|1",
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "Value to set for the SEL0 bit.",
        .group = 0
    },
    {
        .name  = "en0",
        .key   = 'h',
        .arg   = "0|1",
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "Value to set for the EN0 bit.",
        .group = 0
    },
    {
        .name  = "prescalar0",
        .key   = 'q',
        .arg   = "1|2|4|8",
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "Value to set for the prescalar for OUT0.",
        .group = 0
    },
    {
        .name  = "prescalar1",
        .key   = 'r',
        .arg   = "1|2|4|8",
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "Value to set for the prescalar for OUT1.",
        .group = 0
    },
    {
        .name  = "div1",
        .key   = 'w',
        .arg   = "0|1",
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "Value to set for the DIV1 bit.",
        .group = 0
    },
    { 0 }
};

const struct argp argps = {
    options,
    parse_opts,
    NULL,
    "Set values in the MUX register of a Maxim DS1077L programmable oscillator."
};

static error_t
parse_opts (int key, char *arg, struct argp_state *state)
{
    mux_args_t* mux_args = state->input;
    uint32_t number = 0;

    switch (key) {
        case 'a':
            /* address is unsigned 8bit integer between 0x58 and 0x5f */
            mux_args->address = strtol (arg, NULL, 16);
            if (mux_args->address < 0x58 | mux_args->address > 0x5f)
                argp_usage (state);
            break;
        case 'd':
            /* path to bus device node is validated in the main program */
            mux_args->bus_dev = arg;
            break;
        case 'p':
            /* value for PDN1 bit, binary digit */
            mux_args->pdn1 = strtol (arg, NULL, 10);
            if (mux_args->pdn1 < 0x0 | mux_args->pdn1 > 0x1)
                argp_usage (state);
            mux_args->pdn1_set = true;
            break;
        case 'q':
            /* value for PDN0 bit, binary digit */
            mux_args->pdn0 = strtol (arg, NULL, 10);
            if (mux_args->pdn0 < 0x0 | mux_args->pdn0 > 0x1)
                argp_usage (state);
            mux_args->pdn0_set = true;
            break;
        case 's':
            /* value for SEL0 bit, binary digit */
            mux_args->sel0 = strtol (arg, NULL, 10);
            if (mux_args->sel0 < 0x0 | mux_args->sel0 > 0x1)
                argp_usage (state);
            mux_args->sel0_set = true;
            break;
        case 'e':
            /*value for EN0 bit, binary digit */
            mux_args->en0 = strtol (arg, NULL, 10);
            if (mux_args->en0 < 0x0 | mux_args->en0 > 0x1)
                argp_usage (state);
            mux_args->en0_set = true;
            break;
        case 'c':
            /* value for prescalar P0, powers of 2 between 1 and 8 */
            mux_args->m0 = strtol (arg, NULL, 10);
            if (! mux_args->m0 & 0x0F)
                argp_usage (state);
            mux_args->m0_set = true;
            break;
        case 'r':
            /* value for prescalar P1, powers of 2 between 1 and 8 */
            mux_args->m1 = strtol (arg, NULL, 10);
            if (! mux_args->m1 & 0x0F)
                argp_usage (state);
            mux_args->m1_set = true;
            break;
        case 'v':
            /* value for DIV1 bit, binary digit */
            mux_args->div1 = strtol (arg, NULL, 10);
            if (mux_args->div1 & 0xFE)
                argp_usage (state);
            mux_args->div1_set = true;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static void
mux_args_dump (mux_args_t *mux_args)
{
    printf ("address: 0x%x\n", mux_args->address);
    printf ("bus-dev: %s\n",   mux_args->bus_dev);
    printf ("pdn1:    %s\n",   mux_args->pdn1 ? "true" : "false");
    printf ("pdn0:    %s\n",   mux_args->pdn0 ? "true" : "false");
    printf ("sel0:    %s\n",   mux_args->sel0 ? "true" : "false");
    printf ("en0:     %s\n",   mux_args->en0  ? "true" : "false");
    printf ("div1:    %s\n",   mux_args->div1 ? "true" : "false");
}

/* Populate a mux_args_t with default values.
 */
void
mux_args_init (mux_args_t *mux_args)
{
    mux_args->address  = DS1077L_ADDR_DEFAULT;
    mux_args->bus_dev  = I2C_BUS_DEVICE;
    mux_args->pdn1     = DS1077L_PDN1_DEFAULT;
    mux_args->pdn1_set = false;
    mux_args->pdn0     = DS1077L_PDN0_DEFAULT;
    mux_args->pdn0_set = false;
    mux_args->sel0     = DS1077L_SEL0_DEFAULT;
    mux_args->sel0_set = false;
    mux_args->en0      = DS1077L_EN0_DEFAULT;
    mux_args->en0_set  = false;
    mux_args->m0       = DS1077L_M0_DEFAULT;
    mux_args->m0_set   = false;
    mux_args->m1       = DS1077L_M1_DEFAULT;
    mux_args->m1_set   = false;
    mux_args->div1     = DS1077L_DIV1_DEFAULT;
    mux_args->div1_set = false;
}

/* Populate a ds1077l mux_t with the data from a mux_args_t. 
 * Only set values that were supplied by the user.
 */
void
mux_from_args (mux_args_t *mux_args, ds1077l_mux_t *mux)
{
    if (mux_args->pdn1_set)
        mux->pdn1 = mux_args->pdn1;
    if (mux_args->pdn0_set)
        mux->pdn0 = mux_args->pdn0;
    if (mux_args->sel0_set)
        mux->sel0 = mux_args->sel0;
    if (mux_args->en0_set)
        mux->en0  = mux_args->en0;
    if (mux_args->m0_set)
        mux->m0   = mux_args->m0;
    if (mux_args->m1_set)
        mux->m1   = mux_args->m1;
    if (mux_args->div1_set)
        mux->div1 = mux_args->div1;
}

/* Compare two ds1077l_mux_t structures.
 * Returns 0 if they match, 1 otherwise.
 */
int
mux_compare (ds1077l_mux_t *first, ds1077l_mux_t *second)
{
    if (first->pdn1 != second->pdn1)
        return 1;
    if (first->pdn0 != second->pdn0)
        return 1;
    if (first->sel0 != second->sel0)
        return 1;
    if (first->en0  != second->en0)
        return 1;
    if (first->m0   != second->m0)
        return 1;
    if (first->m1   != second->m1)
        return 1;
    if (first->div1 != second->div1)
        return 1;
    return 0;
}
int
main (int argc, char *argv[])
{
    int fd = 0;
    /* argument structure populated with defaults */
    mux_args_t mux_args = {0};
    mux_args_init (&mux_args);
    ds1077l_mux_t mux_new = {0};
    ds1077l_mux_t mux_current = {0};

    if (argp_parse (&argps, argc, argv, 0, NULL, &mux_args)) {
        perror ("argp_parse: \n");
        exit (1);
    }
    mux_args_dump (&mux_args);
    fd = handle_get (mux_args.bus_dev, mux_args.address);
    if (fd == -1) {
        perror ("handle_get: ");
        exit (1);
    }
    /* get current register state and display to user */
    if (mux_get (fd, &mux_current)) {
        perror ("mux_set: ");
        exit (1);
    }
    printf ("Current MUX register state:\n");
    mux_pretty (&mux_current);
    /* determine whether any values are bing changed, exit if not */
    mux_new = mux_current;
    mux_from_args (&mux_args, &mux_new);
    printf ("Requested MUX register state:\n");
    mux_pretty (&mux_new);
    if (mux_compare (&mux_current, &mux_new) == 0) {
        printf ("MUX register already in requested state. No change necessary.\n");
        exit (0);
    }

    printf ("dones\n");
    exit (1);
    /*
    printf ("Setting device 0x%x on bus %s to:\n",
            mux_args.address, mux_args.bus_dev);
    mux_pretty (&mux_new);
    if (mux_set (fd, &mux_new)) {
        perror ("mux_set: ");
        exit (1);
    }
    exit (0);
    */
}

