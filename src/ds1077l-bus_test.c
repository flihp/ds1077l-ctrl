#include "ds1077l-bus.h"

#include <stdint.h>
#include <stdio.h>

int main(void)
{
    ds1077l_bus_t bus_obj0 = {0};
    /*  BUS pack / unpack  */
    /*  test results: wc: 0, address 0x5a  */
    uint8_t bus0 = 0b00001010;
    printf("expect address: 0x5a, wc: false\n");
    printf("bus0:    0x%x\n", bus0);
    printf("address: 0x%x\n", ADDRESS_UNPACK(bus0));
    printf("wc:      %s\n", WC_UNPACK(bus0) ? "true" : "false");
    /*  test results: wc: 1, address 0x5d  */
    uint8_t bus1 = 0b00001101;
    printf("expect address: 0x5d, wc: false\n");
    printf("bus1:    0x%x\n", bus1);
    printf("address: 0x%x\n", ADDRESS_UNPACK(bus1));
    printf("wc:      %s\n", WC_UNPACK(bus1) ? "true" : "false");
    /*  test results: wc: 0, address 0x5b  */
    uint8_t bus2 = 0b00000011;
    printf("expect address: 0x5b, wc: true\n");
    printf("bus2:    0x%x\n", bus2);
    printf("address: 0x%x\n", ADDRESS_UNPACK(bus2));
    printf("wc:      %s\n", WC_UNPACK(bus2) ? "true" : "false");

    bus_obj0.address = 0x5a;
    bus_obj0.wc = false;
    printf("packing address: 0x%x and wc: %s\n",
           bus_obj0.address, bus_obj0.wc ? "true" : "false");
    printf("expect bus:     0xa\n");
    printf("packed address: 0x%x\n", ADDRESS_PACK(bus_obj0.address));
    printf("packed wc:      0x%x\n", WC_PACK(bus_obj0.wc));
    printf("bus:            0x%x\n", BUS_PACK((&bus_obj0)));
    bus_obj0.address = 0x5d;
    bus_obj0.wc = false;
    printf("packing address: 0x%x and wc: %s\n",
           bus_obj0.address, bus_obj0.wc ? "true" : "false");
    printf("expect bus:     0xd\n");
    printf("packed address: 0x%x\n", ADDRESS_PACK(bus_obj0.address));
    printf("packed wc:      0x%x\n", WC_PACK(bus_obj0.wc));
    printf("bus:            0x%x\n", BUS_PACK((&bus_obj0)));
    bus_obj0.address = 0x5b;
    bus_obj0.wc = true;
    printf("packing address: 0x%x and wc: %s\n",
           bus_obj0.address, bus_obj0.wc ? "true" : "false");
    printf("expect bus:     0x3\n");
    printf("packed address: 0x%x\n", ADDRESS_PACK(bus_obj0.address));
    printf("packed wc:      0x%x\n", WC_PACK(bus_obj0.wc));
    printf("bus:            0x%x\n", BUS_PACK((&bus_obj0)));

}
