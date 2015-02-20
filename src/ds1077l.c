#include "ds1077l.h"

#include <linux/i2c-dev.h>
#include <stdio.h>
#include <fcntl.h>

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
