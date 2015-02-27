#include <stdbool.h>
#include <stdint.h>

/* properties */
#define DS1077L_ADDR_DEFAULT 0x58
#define DS1077L_WC_DEFAULT   false

#define COMMAND_BUS 0x0d

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
#define WC_UNPACK(bus)      ((bus & 0x08) ? true : false)
#define ADDRESS_PACK(address) (address & 0x07)
#define WC_PACK(wc) (((wc ? 1 : 0) & 0x01) << 3)
#define BUS_PACK(bus)       (ADDRESS_PACK(bus->address) | WC_PACK(bus->wc))

typedef struct ds1077l_bus {
    bool wc;
    uint8_t address;
} ds1077l_bus_t;
