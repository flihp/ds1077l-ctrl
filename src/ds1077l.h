#include <stdint.h>
#include <stdbool.h>

/* properties */
#define DS1077L_ADDR_DEFAULT 0x58

/* commands */
#define COMMAND_DIV 0x01
#define COMMAND_MUX 0x02
#define COMMAND_BUS 0x0d
#define COMMAND_E2_WRITE 0x3f

typedef struct ds1077l_bus {
    bool wc;
    uint8_t address;
} ds1077l_bus_t;

int handle_get(char* dev, uint8_t addr);
int bus_get(int fd, ds1077l_bus_t* bus);
void bus_pretty(ds1077l_bus_t* bus);
