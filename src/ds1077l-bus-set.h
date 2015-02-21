#include <stdbool.h>
#include <stdint.h>

typedef struct bus_args {
    uint8_t address;
    uint8_t new_addr;
    bool new_addr_set;
    char *bus;
    bool write_on_change;
    bool write_on_change_set;
} bus_args_t;

