#include "ds1077l.h"

#include <stdint.h>
#include <stdio.h>

int main(void)
{
    uint32_t div = 0;

    /*  div: 0x2  */
    div = 0x0000;
    printf("div: 0x%x\n", DIV_UNPACK(div));

    /*  div: 0x4  */
    printf("should be: 0x4\n");
    printf("div: 0x%x\n", DIV_UNPACK(0x8000));
    printf("div: 0x%x\n", DIV_UNPACK(0x9500));
    printf("div: 0x%x\n", DIV_UNPACK(0x8f00));

    /*  div: 0xc  */
    printf("should be: 0xc\n");
    printf("div: 0x%x\n", DIV_UNPACK(0xc000));
    printf("div: 0x%x\n", DIV_UNPACK(0xcc00));

    /*  div: 0x45  */
    printf("should be: 0x45 + 2\n");
    printf("div: 0x%x\n", DIV_UNPACK(0x4011));
    printf("div: 0x%x\n", DIV_UNPACK(0x7f11));

    return 0;
}
