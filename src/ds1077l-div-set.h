#include "ds1077l.h"

#include <stdbool.h>
#include <stdint.h>

/* Default values for the DIV register. The packed representation is the value
 * as seen in the device register. The unpacked value is the value as
 * interpreted according to table 5 in the data sheet.
 */
#define DS1077L_DIV_DEFAULT_PACKED 0x0
#define DS1077L_DIV_DEFAULT_UNPACKED DIV_UNPACK(DS1077L_DIV_DEFAULT_PACKED);

typedef struct div_args {
    ds1077l_common_args_t common_args;
    uint16_t divider;
    bool divider_set;
} div_args_t;
