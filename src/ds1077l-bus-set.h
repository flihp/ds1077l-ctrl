#include "ds1077l.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct bus_args {
    ds1077l_common_args_t common_args;
    bool get;
    bool set;
    uint8_t new_addr;
    bool new_addr_set;
    bool wc;
    bool wc_set;
} bus_args_t;

