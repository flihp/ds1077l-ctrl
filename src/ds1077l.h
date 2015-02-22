#include <stdint.h>
#include <stdbool.h>

/* Addresses, structures and other shit as adapted from the spec sheet for the
 * Maxim DS1077L oscillator. For definitive data consult the spec sheet:
 * http://datasheets.maximintegrated.com/en/ds/DS1077L.pdf
 */

/* stuff */
#define I2C_BUS_DEVICE "/dev/i2c-1"

/* properties */
#define DS1077L_ADDR_DEFAULT 0x58
#define DS1077L_WC_DEFAULT   true
#define DS1077L_N_DEFAULT    0X2

/* commands */
#define COMMAND_DIV 0x01
#define COMMAND_MUX 0x02
#define COMMAND_BUS 0x0d
#define COMMAND_E2_WRITE 0x3f

/* bus register (un)?pack
 * Diagram taken from the spec sheet, page 7.
 *
 * BUS WORD
 * +-----------------+----+----+----+----+----+----+----+----+
 * | NAME            | -  | -  | -  | -  | WC | A2 | A1 | A0 |
 * +-----------------+----+----+----+----+----+----+----+----+
 * | Factory Default | 0* | 0* | 0* | 0* | 0  | 0  | 0  | 0  |
 * +-----------------+----+----+----+----+----+----+----+----+
 * * These bits are reserved and must be set to zero.
 *
 * NOTE: the WC bit is reversed from what we would normally expect. When it's
 *       set to 0 the DS1077L will write registers to EEPROM on any changes
 *       and when it's set to 1 changes are written to EEPROM only on a WRITE
 *       command. When we pack and unpack this bit we set the boolean in the
 *       ds1077l_bus_t structure to true if this bit is 0 and false if it's 1.
 */
#define ADDRESS_UNPACK(bus) (0x07 & bus | DS1077L_ADDR_DEFAULT)
#define WC_UNPACK(bus)      ((bus & 0x08) ? false : true)
#define ADDRESS_PACK(address) (address & 0x07)
#define WC_PACK(wc) (((wc ? 0 : 1) & 0x01) << 3)
#define BUS_PACK(bus)       (ADDRESS_PACK(bus->address) | WC_PACK(bus->wc))

/* div register (un)?pack
 * Diagram taken from the spec sheet page 6.
 *
 * DIV WORD
 * MSB                                 LSB | MSB                         LSB |
 * +----+----+----+----+----+----+----+----+----+----+---+---+---+---+---+---+
 * | N9 | N8 | N7 | N6 | N5 | N4 | N3 | N2 | N1 | N0 | X | X | X | X | X | X |
 * +----+----+----+----+----+----+----+----+----+----+---+---+---+---+---+---+
 *              first data byte            |         second data byte
 *
 * The weird thing about this is that the first data byte comes off the bus
 * first. So the integer we get has these two bytes packed in the reverse
 * order.
 *
 * Also see note on adding 2 to the N value (bottom of page 6).
 */
#define DIV_UNPACK(div) (((div & 0xFF) << 2 | (div & 0xc000) >> 14) + 2)
#define DIV_PACK(div)   ((((div - 2) >> 2) & 0xFF) | ((div - 2) & 0x3) << 14) 

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
#define PDN0_UNPACK(mux) (mux & 0x20 ? true : false)
#define SEL0_UNPACK(mux) (mux & 0x10 ? true : false)
#define EN0_UNPACK(mux)  (mux & 0x08 ? true : false)
#define M0_UNPACK(mux)   decode_prescalar((mux & 0x6) >> 1)
#define M1_UNPACK(mux)   decode_prescalar((mux & 0x8000) >> 7 | (mux & 0x1) << 1)
#define DIV1_UNPACK(mux) (mux & 0x4000 ? true : false)

typedef struct ds1077l_bus {
    bool wc;
    uint8_t address;
} ds1077l_bus_t;

typedef struct ds1077l_mux {
    bool pdn1;
    bool pdn0;
    bool sel0;
    bool en0;
    uint8_t m0;
    uint8_t m1;
    bool div1;
} ds1077l_mux_t;

typedef struct ds1077l_div {
    uint16_t n;
} ds1077l_div_t;

int handle_get(char* dev, uint8_t addr);
int bus_get(int fd, ds1077l_bus_t* bus);
int bus_set (int fd, ds1077l_bus_t *bus);
void bus_pretty(ds1077l_bus_t* bus);
int mux_get(int fd, ds1077l_mux_t* mux);
void mux_pretty(ds1077l_mux_t* mux);
int div_get(int fd, ds1077l_div_t* div);
void div_pretty(ds1077l_div_t* div);
inline uint8_t decode_prescalar(uint8_t m);
