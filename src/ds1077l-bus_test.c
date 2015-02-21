#include "ds1077l.h"

#include <stdint.h>
#include <stdio.h>

int main(void)
{
    /*  BUS pack / unpack  */
    /*  test results: wc: 0, address 0x5a  */
    uint8_t bus0 = 0b00001010;
    printf("bus0:    0x%x\n", bus0);
    printf("address: 0x%x\n", ADDRESS_UNPACK(bus0));
    printf("wc:      0x%x\n", WC_UNPACK(bus0));
    /*  test results: wc: 1, address 0x5d  */
    uint8_t bus1 = 0b00001101;
    printf("bus1:    0x%x\n", bus1);
    printf("address: 0x%x\n", ADDRESS_UNPACK(bus1));
    printf("wc:      0x%x\n", WC_UNPACK(bus1));
    /*  test results: wc: 0, address 0x5b  */
    uint8_t bus2 = 0b00000011;
    printf("bus2:    0x%x\n", bus2);
    printf("address: 0x%x\n", ADDRESS_UNPACK(bus2));
    printf("wc:      0x%x\n", WC_UNPACK(bus2));
}
