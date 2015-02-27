#include "ds1077l-mux.h"

#include <linux/i2c-dev.h>
#include <stdio.h>

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

int
mux_get (int fd, ds1077l_mux_t* mux)
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

void
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
