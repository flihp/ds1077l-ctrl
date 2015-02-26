#include "ds1077l.h"

#include <stdbool.h>
#include <stdint.h>

typedef struct div_args {
    ds1077l_common_args_t common_args;
    uint16_t divider;
    bool divider_set;
} div_args_t;
