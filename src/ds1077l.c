#include "ds1077l.h"

#include <linux/i2c-dev.h>
#include <stdio.h>
#include <fcntl.h>

static uint8_t decode_prescalar(uint8_t m);
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

/* Populate bus data structure with contents of BUS register on ds1077l device
   represented by fd parameter.
 */
int bus_get(int fd, ds1077l_bus_t* bus)
{
    int ret = 0;

    ret = i2c_smbus_read_byte_data(fd, COMMAND_BUS); 
    if (ret == -1)
        return ret;
    /* wc bit is the 4th bit in the first byte */
    bus->wc = 0x07 & ret;
    /* address is 0x58 + low 3 bits in the first byte */
    bus->address = 0x08 & ret | DS1077L_ADDR_DEFAULT;
    return 0;
}

/* Pretty print data from parameter BUS structure.
 */
void bus_pretty(ds1077l_bus_t* bus)
{
    if (bus == NULL)
        return;
    printf("BUS:\n");
    printf("  Address: %#x\n", bus->address);
    printf("    A2: %#x\n", bus->address & 0x4);
    printf("    A1: %#x\n", bus->address & 0x2);
    printf("    A0: %#x\n", bus->address & 0x1);
    printf("  WC: %#x\n", bus->wc);
}

static int mux_from_int(ds1077l_mux_t* mux, int32_t word)
{
    if (mux == NULL)
        return -1;
    /* See page 5 from the DS1077L data sheet for MUX WORD format.
     * First byte read is the least significant byte in the int32_t.
     */
    mux->pdn1 = word & 0x40 ? true : false;
    mux->pdn0 = word & 0x20 ? true : false;
    mux->sel0 = word & 0x10 ? true : false;
    mux->en0  = word & 0x08 ? true : false;
    mux->m0   = decode_prescalar((word & 0x6)>>1);
    /* m1 is wacky because the 1M bits span the byte boundary. 1M1 is the LSB
     * of the first byte and 1M0 is the MSB of the second byte.
     */
    mux->m1   = decode_prescalar((word & 0x8000)>>7 | (word & 0x1)<<1);
    mux->div1 = word & 0x4000 ? true : false;
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

/* Decode value of the prescalar 'M'. see table 4 from DS1077L spec sheet.
 */
static uint8_t decode_prescalar(uint8_t m)
{
    switch (m) {
    case 0:
        return 1;
    case 1:
        return 2;
    case 2:
        return 4;
    case 4:
        return 8;
    default:
        return -1;
    }
}
