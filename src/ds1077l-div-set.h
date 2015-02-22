#include "ds1077l.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct div_args {
    uint8_t address;
    char *bus_dev;
    uint16_t divider;
    bool divider_set;
} div_args_t;
