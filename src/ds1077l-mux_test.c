#include "ds1077l.h"
#include "ds1077l-mux.h"

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
    printf ("m1 unpack: 0x8001\n");
    printf ("m1:        0x%x\n", M1_UNPACK(0x8001));
    printf ("m1 unpack: 0x0001\n");
    printf ("m1:        0x%x\n", M1_UNPACK(0x0001));
    printf ("m1 unpack: 0x8000\n");
    printf ("m1:        0x%x\n", M1_UNPACK(0x8000));
    printf ("m1 unpack: 0x0000\n");
    printf ("m1:        0x%x\n", M1_UNPACK(0x0000));
    printf("m1:   0x%x\n", M1_UNPACK(mux));
    /* DIV1: 1 */
    printf("div1: 0x%x\n", DIV1_UNPACK(mux));

    /* PACK */
    printf ("packed:\n");
    /* PDN1: bit 7 in first byte */
    printf ("  pdn1: 0x%x\n", PDN1_PACK(true));
    /* PDN0: bit 6 in first byte */
    printf ("  pdn0: 0x%x\n", PDN0_PACK(true));
    /* SEL0: bit 5 in first byte */
    printf ("  sel0: 0x%x\n", SEL0_PACK(true));
    /* EN0:  bit 4 in first byte */
    printf ("  en0:  0x%x\n", EN0_PACK(true));
    /* */
    printf ("  encoding prescalar: 0x%x\n", 0x8);
    printf ("  encode_prescalar: 0x%x\n", encode_prescalar(0x8));
    printf ("  encoding prescalar: 0x%x\n", 0x4);
    printf ("  encode_prescalar: 0x%x\n", encode_prescalar(0x4));
    printf ("  encoding prescalar: 0x%x\n", 0x2);
    printf ("  encode_prescalar: 0x%x\n", encode_prescalar(0x2));
    printf ("  encoding prescalar: 0x%x\n", 0x1);
    printf ("  encode_prescalar: 0x%x\n", encode_prescalar(0x1));
    /* M0:   bit 3 & 2 in first byte */
    printf ("  m0:   0x%x\n", M0_PACK(0x8));
    /* M1:   first bit in first byte, last bit in second byte */
    printf ("  packing m1: 0x1\n");
    printf ("  m1:   0x%x\n", M1_PACK(0x1));
    printf ("  packing m1: 0x2\n");
    printf ("  m1:   0x%x\n", M1_PACK(0x2));
    printf ("  packing m1: 0x4\n");
    printf ("  m1:   0x%x\n", M1_PACK(0x4));
    printf ("  packing m1: 0x8\n");
    printf ("  m1:   0x%x\n", M1_PACK(0x8));
    /* DIV1: 8th bit in second byte */
    printf ("  div1: 0x%x\n", DIV1_PACK(true));
}
