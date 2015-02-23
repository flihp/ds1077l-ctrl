#include <stdbool.h>
#include <stdint.h>

typedef struct dump_args {
    char *bus_dev;
    uint8_t address;
    bool verbose;
} dump_args_t;
