#include "ds1077l.h"
#include "ds1077l-mux.h"

#include <argp.h>
#include <linux/i2c-dev.h>
#include <stdio.h>
#include <stdlib.h>

static error_t parse_opts (int key, char *arg, struct argp_state *state);
static void mux_args_init (mux_args_t *mux_args);

/* arguments */
const struct argp_option options[] = {
    {
        .name  = "get",
        .key   = 'g',
        .arg   = 0,
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "Get the contents of the MUX register.",
        .group = 1
    },
    {
        .name  = "set",
        .key   = 's',
        .arg   = 0,
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "Set the contents of the MUX register.",
        .group = 1
    },
    {
        .name  = "pdn1",
        .key   = 'e',
        .arg   = "0|1",
        .flags = 0,
        .doc   = "Value to set for the PDN1 bit.",
        .group = 2
    },
    {
        .name  = "pdn0",
        .key   = 'f',
        .arg   = "0|1",
        .flags = 0,
        .doc   = "Value to set for the PDN0 bit.",
        .group = 2
    },
    {
        .name  = "sel0",
        .key   = 'h',
        .arg   = "0|1",
        .flags = 0,
        .doc   = "Value to set for the SEL0 bit.",
        .group = 2
    },
    {
        .name  = "en0",
        .key   = 'i',
        .arg   = "0|1",
        .flags = 0,
        .doc   = "Value to set for the EN0 bit.",
        .group = 2
    },
    {
        .name  = "p0",
        .key   = 'q',
        .arg   = "1|2|4|8",
        .flags = 0,
        .doc   = "Value to set for the prescalar for OUT0. This is the "
                 "combination and mapping of the 0M0 and 0M1 bits.",
        .group = 2
    },
    {
        .name  = "p1",
        .key   = 'r',
        .arg   = "1|2|4|8",
        .flags = 0,
        .doc   = "Value to set for the prescalar for OUT2. This is the "
                 "combination and mapping of the 1M0 and 1M1 bits.",
        .group = 2
    },
    {
        .name  = "div1",
        .key   = 'w',
        .arg   = "0|1",
        .flags = 0,
        .doc   = "Value to set for the DIV1 bit.",
        .group = 2
    },
    { 0 }
};

const struct argp_child argp_children[] = {
    {
        .argp   = &common_argp,
        .flags  = 0,
        .header = NULL,
        .group  = 0
    },
    { 0 }
};

const struct argp argps = {
    .options     = options,
    .parser      = parse_opts,
    .args_doc    = NULL,
    .doc         = "Set values in the MUX register of a Maxim DS1077L "
                   "programmable oscillator.",
    .children    = argp_children,
    .help_filter = NULL,
    .argp_domain = NULL,
};

static error_t
parse_opts (int key, char *arg, struct argp_state *state)
{
    mux_args_t* mux_args = state->input;

    switch (key) {
        case 'g':
            mux_args->get = true;
            break;
        case 's':
            mux_args->set = true;
            break;
        case 'e':
            /* value for PDN1 bit, binary digit */
            mux_args->pdn1 = strtol (arg, NULL, 10);
            if (mux_args->pdn1 < 0x0 | mux_args->pdn1 > 0x1)
                argp_usage (state);
            mux_args->pdn1_set = true;
            break;
        case 'f':
            /* value for PDN0 bit, binary digit */
            mux_args->pdn0 = strtol (arg, NULL, 10);
            if (mux_args->pdn0 < 0x0 | mux_args->pdn0 > 0x1)
                argp_usage (state);
            mux_args->pdn0_set = true;
            break;
        case 'h':
            /* value for SEL0 bit, binary digit */
            mux_args->sel0 = strtol (arg, NULL, 10);
            if (mux_args->sel0 < 0x0 | mux_args->sel0 > 0x1)
                argp_usage (state);
            mux_args->sel0_set = true;
            break;
        case 'i':
            /*value for EN0 bit, binary digit */
            mux_args->en0 = strtol (arg, NULL, 10);
            if (mux_args->en0 < 0x0 | mux_args->en0 > 0x1)
                argp_usage (state);
            mux_args->en0_set = true;
            break;
        case 'q':
            /* value for prescalar P0, powers of 2 between 1 and 8 */
            mux_args->m0 = strtol (arg, NULL, 10);
            if (! (mux_args->m0 == 1 || mux_args->m0 == 2 || \
                   mux_args->m0 == 4 || mux_args->m0 == 8))
                argp_usage (state);
            mux_args->m0_set = true;
            break;
        case 'r':
            /* value for prescalar P1, powers of 2 between 1 and 8 */
            mux_args->m1 = strtol (arg, NULL, 10);
            if (! (mux_args->m1 == 1 || mux_args->m1 == 2 || \
                   mux_args->m1 == 4 || mux_args->m1 == 8))
                argp_usage (state);
            mux_args->m1_set = true;
            break;
        case 'w':
            /* value for DIV1 bit, binary digit */
            mux_args->div1 = strtol (arg, NULL, 10);
            if (mux_args->div1 & 0xFE)
                argp_usage (state);
            mux_args->div1_set = true;
            break;
        case ARGP_KEY_INIT:
            mux_args_init (mux_args);
            state->child_inputs[0] = &(mux_args->common_args);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static void
mux_args_dump (mux_args_t *mux_args)
{
    printf ("Use provided options:\n");
    printf ("  get:     %s\n",   mux_args->get ? "true" : "false");
    printf ("  set:     $s\n",   mux_args->set ? "true" : "false");
    printf ("  address: 0x%x\n", mux_args->common_args.address);
    printf ("  bus-dev: %s\n",   mux_args->common_args.bus_dev);
    printf ("  verbose: %s\n",   mux_args->common_args.verbose ? "true" : "false");
    printf ("  pdn1:    %s\n",   mux_args->pdn1 ? "true" : "false");
    printf ("  pdn0:    %s\n",   mux_args->pdn0 ? "true" : "false");
    printf ("  sel0:    %s\n",   mux_args->sel0 ? "true" : "false");
    printf ("  en0:     %s\n",   mux_args->en0  ? "true" : "false");
    printf ("  div1:    %s\n",   mux_args->div1 ? "true" : "false");
}

static int
mux_from_int (ds1077l_mux_t* mux, int32_t word)
{
    if (mux == NULL)
        return -1;
    /* See page 5 from the DS1077L data sheet for MUX WORD format.
     * First byte read is the least significant byte in the int32_t.
     */
    mux->pdn1 = PDN1_UNPACK(word);
    mux->pdn0 = PDN0_UNPACK(word);
    mux->sel0 = SEL0_UNPACK(word);
    mux->en0  = EN0_UNPACK(word);
    mux->m0   = M0_UNPACK(word);
    /* m1 is wacky because the 1M bits span the byte boundary. 1M1 is the LSB
     * of the first byte and 1M0 is the MSB of the second byte.
     */
    mux->m1   = M1_UNPACK(word);
    mux->div1 = DIV1_UNPACK(word);
    return 0;
}

/* Populate a mux_args_t with default values.
 */
static void
mux_args_init (mux_args_t *mux_args)
{
    mux_args->get      = false;
    mux_args->set      = false;
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
static void
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
static int
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

static int
mux_get (int fd, ds1077l_mux_t* mux)
{
    int32_t ret = 0;

    ret = i2c_smbus_read_word_data(fd, COMMAND_MUX);
    if (ret == -1)
        return ret;
    mux_from_int(mux, ret);
    return 0;
}

static int
mux_set (int fd, ds1077l_mux_t *mux)
{
    int32_t ret = 0;
    uint16_t mux_word = 0;

    mux_word = (PDN1_PACK(mux->pdn1) | PDN0_PACK(mux->pdn0) | \
                SEL0_PACK(mux->sel0) | EN0_PACK(mux->en0)   | \
                M0_PACK(mux->m0)     | M1_PACK(mux->m1)     | \
                DIV1_PACK(mux->div1));
    ret = i2c_smbus_write_word_data (fd, COMMAND_MUX, mux_word);
    if (ret == -1)
        return -1;
    return 0;
}

static void
mux_pretty (ds1077l_mux_t* mux)
{
    if (mux == NULL)
        return;
    printf("MUX:\n");
    printf("  PDN1: %s\n", mux->pdn1 ? "true" : "false");
    printf("  PDN0: %s\n", mux->pdn0 ? "true" : "false");
    printf("  SEL0: %s\n", mux->sel0 ? "true" : "false");
    printf("  EN0:  %s\n", mux->en0  ? "true" : "false");
    printf("  M0:   %d\n", mux->m0);
    printf("  M1:   %d\n", mux->m1);
    printf("  DIV1: %s\n", mux->div1 ? "true" : "false");
}

int
main (int argc, char *argv[])
{
    int fd = 0;
    mux_args_t mux_args = {0};
    ds1077l_mux_t mux_new = {0};
    ds1077l_mux_t mux_current = {0};

    if (argp_parse (&argps, argc, argv, 0, NULL, &mux_args)) {
        perror ("argp_parse: \n");
        exit (1);
    }
    if (mux_args.get && mux_args.set) {
        fprintf (stderr, "Select either 'get' or 'set', not both.\n");
        exit (1);
    }
    if (! (mux_args.get || mux_args.set)) {
        fprintf (stderr, "Must select either 'get' or 'set'.\n");
        exit (1);
    }
    if (mux_args.common_args.verbose)
        mux_args_dump (&mux_args);
    fd = handle_get (mux_args.common_args.bus_dev,
                     mux_args.common_args.address);
    if (fd == -1) {
        perror ("handle_get: ");
        exit (1);
    }
    if (mux_args.common_args.verbose)
        printf ("Querying state of MUX register for device 0x%x on bus %s\n",
               mux_args.common_args.address, mux_args.common_args.bus_dev);
    /* get current register state and display to user */
    if (mux_get (fd, &mux_current)) {
        perror ("mux_set: ");
        exit (1);
    }
    if (mux_args.get || mux_args.common_args.verbose)
        mux_pretty (&mux_current);
    if (mux_args.get)
        exit (0);
    /* determine whether any values are bing changed, exit if not */
    mux_new = mux_current;
    mux_from_args (&mux_args, &mux_new);
    if (mux_args.common_args.verbose) {
        printf ("Requested MUX register state:\n");
        mux_pretty (&mux_new);
    }
    if (mux_compare (&mux_current, &mux_new) == 0) {
        printf ("MUX register already in requested state. No change necessary.\n");
        exit (0);
    }
    if (mux_args.common_args.verbose) {
        printf ("Setting MUX register for device 0x%x on bus %s to:\n",
                mux_args.common_args.address, mux_args.common_args.bus_dev);
        mux_pretty (&mux_new);
    }
    if (mux_set (fd, &mux_new)) {
        perror ("mux_set: ");
        exit (1);
    }
    exit (0);
}
