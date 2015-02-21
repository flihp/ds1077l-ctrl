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
void bus_pretty(ds1077l_bus_t* bus);
int mux_get(int fd, ds1077l_mux_t* mux);
void mux_pretty(ds1077l_mux_t* mux);
int div_get(int fd, ds1077l_div_t* div);
void div_pretty(ds1077l_div_t* div);
