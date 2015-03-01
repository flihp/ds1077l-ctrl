#ifndef _DS1077L_H_
#define _DS1077L_H_

#include <argp.h>
#include <stdint.h>
#include <stdbool.h>

/* Addresses, structures and other shit as adapted from the spec sheet for the
 * Maxim DS1077L oscillator. For definitive data consult the spec sheet:
 * http://datasheets.maximintegrated.com/en/ds/DS1077L.pdf
 */

/* stuff */
#define I2C_BUS_DEVICE "/dev/i2c-1"

typedef struct ds1077l_common_args {
    uint16_t address;
    char *bus_dev;
    bool verbose;
} ds1077l_common_args_t;

/* If memory serves I'm not supposed to do this. */
extern const struct argp common_argp;
extern const struct argp_option common_options[];

int handle_get(char* dev, uint8_t addr);
error_t parse_common_opts (int key, char *arg, struct argp_state *state);
void dump_common_opts (ds1077l_common_args_t* common_args);

#endif // #ifndef _DS1077L_H_
