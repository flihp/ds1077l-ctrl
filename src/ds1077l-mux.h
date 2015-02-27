#include <stdbool.h>
#include <stdint.h>

/* Command for interacting with the MUX register */
#define COMMAND_MUX 0x02

/* MUX WORD */
#define DS1077L_PDN1_DEFAULT 0x0
#define DS1077L_PDN0_DEFAULT 0x0
#define DS1077L_SEL0_DEFAULT 0x1
#define DS1077L_EN0_DEFAULT  0x1
#define DS1077L_M0_DEFAULT   0x0
#define DS1077L_M1_DEFAULT   0x0
#define DS1077L_DIV1_DEFAULT 0x0

/* mux register (un)?pack
 * Diagram taken from the spec sheet page 5.
 *
 * MUX_WORD
 *                   | MSB                                           LSB |
 * +-----------------+------+------+------+------+-----+-----+-----+-----+
 * | Name            |  *   | PDN1 | PDN0 | SEL0 | EN0 | 0M1 | 0M0 | 1M1 |
 * +-----------------+------+------+------+------+-----+-----+-----+-----+
 * | Default Setting |   0  |   0  |   0  |   1  |  1  |  0  |  0  |  0  |
 * +-----------------+------+------+------+------+-----+-----+-----+-----+
 *                                    first data byte
 *
 *                   | MSB                                           LSB |
 *                   +------+------+------+------+-----+-----+-----+-----+
 *                   |  1M0 | DIV1 |   -  |   -  |  -  |  -  |  -  |  -  |
 *                   +------+------+------+------+-----+-----+-----+-----+
 *                   |   0  |   0  |   X  |   X  |  X  |  X  |  X  |  X  |
 *                   +------+------+------+------+-----+-----+-----+-----+
 *                                    second data byte
 * Also see note on mapping of values for prescalars (*M*) to the divisor in
 * the oscillator.
 */
#define PDN1_UNPACK(mux) (mux & 0x40 ? true : false)
#define PDN1_PACK(pdn1)  (pdn1 ? 0x0040 : 0x0000)
#define PDN0_UNPACK(mux) (mux & 0x20 ? true : false)
#define PDN0_PACK(pdn0)  (pdn0 ? 0x0020 : 0x0000)
#define SEL0_UNPACK(mux) (mux & 0x10 ? true : false)
#define SEL0_PACK(sel0)  (sel0 ? 0x0010 : 0x0000)
#define EN0_UNPACK(mux)  (mux & 0x08 ? true : false)
#define EN0_PACK(en0)    (en0 ? 0x0008 : 0x0000)
#define M0_UNPACK(mux)   decode_prescalar((mux & 0x6) >> 1)
#define M0_PACK(m0)      (encode_prescalar(m0) << 1)
#define M1_UNPACK(mux)   decode_prescalar(((mux & 0x8000) >> 15) | ((mux & 0x1) << 1))
#define M1_PACK(m1)      (((encode_prescalar(m1) & 0x2) >> 1) | \
                          ((encode_prescalar(m1) & 0x1) << 15))
#define DIV1_UNPACK(mux) (mux & 0x4000 ? true : false)
#define DIV1_PACK(div1)  (div1 ? 0x4000 : 0x0000)

typedef struct ds1077l_mux {
    bool pdn1;
    bool pdn0;
    bool sel0;
    bool en0;
    uint8_t m0;
    uint8_t m1;
    bool div1;
} ds1077l_mux_t;

int mux_get(int fd, ds1077l_mux_t* mux);
int mux_set(int fd, ds1077l_mux_t *mux);
void mux_pretty(ds1077l_mux_t* mux);
inline uint8_t encode_prescalar(uint8_t m);
inline uint8_t decode_prescalar(uint8_t m);
