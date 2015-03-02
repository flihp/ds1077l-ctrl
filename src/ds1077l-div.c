#include "ds1077l-div.h"

#include <argp.h>
#include <linux/i2c-dev.h>
#include <stdlib.h>

static error_t parse_opts (int key, char *arg, struct argp_state *state);

const struct argp_option options[] = {
    {
        .name  = "get",
        .key   = 'g',
        .arg   = 0,
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "Get the contents of the DIV register.",
        .group = 1
    },
    {
        .name  = "set",
        .key   = 's',
        .arg   = 0,
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "Set the contents of the BUS register.",
        .group = 1
    },
    {
        .name  = "divider",
        .key   = 'n',
        .arg   = "2-1025",
        .flags = 0,
        .doc   = "Value of the programmable divider on OUT1.",
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
    .doc         = "Set values in the DIV register of a Maxim DS1077L "
                   "programmable oscillator.",
    .children    = argp_children,
    .help_filter = NULL,
    .argp_domain = NULL
};

static error_t
parse_opts (int key, char *arg, struct argp_state *state)
{
    div_args_t* div_args = state->input;

    switch (key) {
        case 'g':
            div_args->get = true;
            break;
        case 's':
            div_args->set = true;
            break;
        case 'n':
            /* n divider value, between 2 and 1025 decimal */
            div_args->divider = strtol (arg, NULL, 10);
            if (div_args->divider < 0x2 | div_args->divider > 0x401)
                argp_usage (state);
            div_args->divider_set = true;
            break;
        case ARGP_KEY_INIT:
            div_args->get = false;
            div_args->set = false;
            div_args->divider = DS1077L_DIV_DEFAULT_UNPACKED;
            div_args->divider_set = false;
            state->child_inputs[0] = &(div_args->common_args);
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static void
div_args_dump (div_args_t *div_args)
{
    printf ("User provided options:\n");
    printf ("  address: 0x%x\n", div_args->common_args.address);
    printf ("  bus-dev: %s\n",   div_args->common_args.bus_dev);
    printf ("  divider: %d\n",   div_args->divider);
    printf ("  verbose: %s\n",   div_args->common_args.verbose ? "true" : "false");
}

/* Get DIV register from the timer and populate the div data structure with it.
 */
static int
div_get(int fd, ds1077l_div_t* div)
{
    uint32_t ret = 0;

    ret = i2c_smbus_read_word_data(fd, COMMAND_DIV);
    if (ret == -1)
        return -1;
    div->n = DIV_UNPACK(ret);
    return 0;
}

/* Set the div register on the device represented by fd with the values in the
 * provided ds1077l_div_t object.
 */
static int
div_set (int fd, ds1077l_div_t *div)
{
    int ret = 0;
    uint16_t div_packed = 0;

    div_packed = DIV_PACK(div->n);
    ret = i2c_smbus_write_word_data (fd, COMMAND_DIV, div_packed);
    if (ret == -1)
        return ret;
    return 0;
}

/* Print DIV structure in human consumable form.
 */
static void
div_pretty (ds1077l_div_t* div)
{
    printf("DIV:\n");
    printf("  N: %d\n", div->n);
}

int
main (int argc, char *argv[])
{
    int fd = 0;
    /* argument structure populated with defaults */
    div_args_t div_args = { 0 };
    ds1077l_div_t div = {0};

    if (argp_parse (&argps, argc, argv, 0, NULL, &div_args)) {
        perror ("argp_parse: \n");
        exit (1);
    }
    if (! (!div_args.get != !div_args.set)) {
        fprintf (stderr, "Select either 'get' or 'set'.\n");
        exit (1);
    }
    if (div_args.get && div_args.divider_set) {
        fprintf (stderr, "--divider cannot be provided with --get.\n");
        exit (1);
    }
    if (div_args.set && !div_args.divider_set) {
        fprintf (stderr, "Must specificy divider value when setting "
                         "register.\n");
        exit (1);
    }
    if (div_args.common_args.verbose)
        div_args_dump (&div_args);
    fd = handle_get (div_args.common_args.bus_dev,
                     div_args.common_args.address);
    if (fd == -1) {
        perror ("handle_get: ");
        exit (1);
    }
    /* get current register state and display to user */
    if (div_args.common_args.verbose)
        printf ("Querying status of DEV register for device 0x%x on bus %s\n",
               div_args.common_args.address, div_args.common_args.bus_dev);
    if (div_get (fd, &div)) {
        perror ("div_set: ");
        exit (1);
    }
    if (div_args.get || div_args.common_args.verbose)
        div_pretty (&div);
    if (div_args.get)
        exit (0);
    /* populate new structure, display to user, and make change */
    div.n = div_args.divider;
    if (div_args.common_args.verbose) {
        printf ("Setting device 0x%x on bus %s to:\n",
                div_args.common_args.address, div_args.common_args.bus_dev);
        div_pretty (&div);
    }
    if (div_set (fd, &div)) {
        perror ("div_set: ");
        exit (1);
    }
    exit (1);
}
