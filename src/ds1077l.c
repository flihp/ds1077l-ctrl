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
        .flags = OPTION_ARG_OPTIONAL,
        .doc   = "The current address of the timer, between 0x58 and 0x5f. "
                 "Defaults to 0x58.",
        .group = 0
    },
    {
        .name  = "bus-dev",
        .key   = 'd',
        .arg   = I2C_BUS_DEVICE,
        .flags = OPTION_ARG_OPTIONAL,
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

static int mux_from_int(ds1077l_mux_t* mux, int32_t word);
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

static int mux_from_int(ds1077l_mux_t* mux, int32_t word)
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

int mux_get(int fd, ds1077l_mux_t* mux)
{
    int32_t ret = 0;

    ret = i2c_smbus_read_word_data(fd, COMMAND_MUX);
    if (ret == -1)
        return ret;
    mux_from_int(mux, ret);
    return 0;
}

int
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

void mux_pretty(ds1077l_mux_t* mux)
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

/* Map divisor values to prescalar.
 */
inline uint8_t
encode_prescalar (uint8_t m)
{
    switch (m) {
    case 1:
        return 0;
    case 2:
        return 1;
    case 4:
        return 2;
    case 8:
        return 3;
    default:
        return -1;
    }
}

/* Map prescalar values to divisor.
 */
inline uint8_t decode_prescalar(uint8_t m)
{
    switch (m) {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 4;
    case 3:
        return 8;
    default:
        return -1;
    }
}


