#include "ds1077l-div.h"

#include <linux/i2c-dev.h>

/* Get DIV register from the timer and populate the div data structure with it.
 */
int div_get(int fd, ds1077l_div_t* div)
{
    uint32_t ret = 0;

    ret = i2c_smbus_read_word_data(fd, COMMAND_DIV);
    if (ret == -1)
        return -1;
    div->n = DIV_UNPACK(ret);
    return 0;
}

/* Set the div register on the device represented by fd with the values in the
 * provided ds1077l_div_t object.
 */
int
div_set (int fd, ds1077l_div_t *div)
{
    int ret = 0;
    uint16_t div_packed = 0;

    div_packed = DIV_PACK(div->n);
    ret = i2c_smbus_write_word_data (fd, COMMAND_DIV, div_packed);
    if (ret == -1)
        return ret;
    return 0;
}

/* Print DIV structure in human consumable form.
 */
void
div_pretty (ds1077l_div_t* div)
{
    printf("DIV:\n");
    printf("  N: %d\n", div->n);
}
