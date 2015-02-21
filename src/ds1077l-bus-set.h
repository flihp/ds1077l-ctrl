#include <stdbool.h>
#include <stdint.h>

typedef struct bus_args {
    uint8_t address;
    uint8_t new_addr;
    char *bus;
    bool write_on_change;
} bus_args_t;

