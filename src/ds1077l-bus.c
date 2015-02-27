#include "ds1077l-bus.h"

#include <linux/i2c-dev.h>
#include <stdio.h>

/* Populate bus data structure with contents of BUS register on ds1077l device
 * represented by fd parameter.
 */
int
bus_get (int fd, ds1077l_bus_t* bus)
{
    int ret = 0;

    ret = i2c_smbus_read_byte_data(fd, COMMAND_BUS); 
    if (ret == -1)
        return ret;
    /* wc bit is the 4th bit in the first byte */
    bus->wc = WC_UNPACK(ret);
    /* address is 0x58 + low 3 bits in the first byte */
    bus->address = ADDRESS_UNPACK(ret);
    return 0;
}

/* Set the bus register on the device represented by fd with the values in the
 * provided ds1077l_bus_t object.
 */
int
bus_set (int fd, ds1077l_bus_t *bus)
{
    int ret = 0;
    uint8_t bus_packed = 0;

    bus_packed = BUS_PACK (bus);
    ret = i2c_smbus_write_byte_data (fd, COMMAND_BUS, bus_packed);
    if (ret == -1)
        return ret;
    return 0;
}

/* Pretty print data from parameter BUS structure.
 */
void
bus_pretty (ds1077l_bus_t* bus)
{
    if (bus == NULL)
        return;
    printf("BUS:\n");
    printf("  Address: %#x\n", bus->address);
    printf("    A2: %#x\n", bus->address & 0x4);
    printf("    A1: %#x\n", bus->address & 0x2);
    printf("    A0: %#x\n", bus->address & 0x1);
    printf("  WC: %s\n", bus->wc ? "true" : "false");
}
