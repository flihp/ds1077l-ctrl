#include "ds1077l.h"

#include <stdint.h>
#include <stdio.h>

int main(void)
{
    uint32_t mux = 0;

    mux = 0x7fff;
    /* PDN1: 1 */
    printf("pdn1: 0x%x\n", PDN1_UNPACK(mux));
    /* PDN0: 1 */
    printf("pdn0: 0x%x\n", PDN0_UNPACK(mux));
    /* SEL0: 1 */
    printf("sel0: 0x%x\n", SEL0_UNPACK(mux));
    /* EN0:  1 */
    printf("en0:  0x%x\n", EN0_UNPACK(mux));
    /* M0:   3 */
    printf("m0:   0x%x\n", M0_UNPACK(mux));
    /* M1:   2 */
    printf("m1:   0x%x\n", M1_UNPACK(mux));
    /* DIV1: 1 */
    printf("div1: 0x%x\n", DIV1_UNPACK(mux));
}
