#include <stdint.h>

/* command for interacting with the DIV register */
#define COMMAND_DIV 0x01

/* Default value for programmable "N" divider */
#define DS1077L_N_DEFAULT    0X2

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

/* Default values for the DIV register. The packed representation is the value
 * as seen in the device register. The unpacked value is the value as
 * interpreted according to table 5 in the data sheet.
 */
#define DS1077L_DIV_DEFAULT_PACKED 0x0
#define DS1077L_DIV_DEFAULT_UNPACKED DIV_UNPACK(DS1077L_DIV_DEFAULT_PACKED);

typedef struct ds1077l_div {
    uint16_t n;
} ds1077l_div_t;

int div_get(int fd, ds1077l_div_t* div);
int div_set (int fd, ds1077l_div_t *div);
void div_pretty(ds1077l_div_t* div);
