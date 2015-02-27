#include <stdbool.h>
#include <stdint.h>

typedef struct mux_args {
    ds1077l_common_args_t common_args;
    bool get;
    bool set;
    bool pdn1;
    bool pdn1_set;
    bool pdn0;
    bool pdn0_set;
    bool sel0;
    bool sel0_set;
    bool en0;
    bool en0_set;
    uint8_t m0;
    bool m0_set;
    uint8_t m1;
    bool m1_set;
    bool div1;
    bool div1_set;
} mux_args_t;
