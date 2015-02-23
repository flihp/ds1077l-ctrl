#include <stdbool.h>
#include <stdint.h>

typedef struct mux_args {
    uint8_t address;
    char *bus_dev;
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
    bool verbose;
} mux_args_t;
