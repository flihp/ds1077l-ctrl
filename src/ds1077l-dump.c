#include "ds1077l.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define DEV "/dev/i2c-1"
#define USAGE "usage: %s /dev/i2c-[0-9]\n"

int main(int argc, char* argv[])
{
    ds1077l_bus_t bus = {0};
    ds1077l_div_t div = {0};
    ds1077l_mux_t mux = {0};
    int fd = 0;
    char* dev_path = NULL;

    switch (argc) {
    case 2:
        dev_path = argv[1];
        break;
    case 1:
        dev_path = DEV;
        break;
    default:
        fprintf(stderr, USAGE, argv[0]);
        exit(1);
        break;
    }

    fd = handle_get(dev_path, 0);
    if (fd == -1) {
        perror("handle_get: ");
        exit(1);
    }
    if (bus_get(fd, &bus) == -1) {
        perror("bus_get: ");
        exit(1);
    }
    bus_pretty(&bus);
    if (mux_get(fd, &mux) == -1) {
        perror("mux_get: ");
        exit(1);
    }
    mux_pretty(&mux);
    if (div_get(fd, &div) == -1) {
        perror("div_get: ");
        exit(1);
    }
    div_pretty(&div);
    exit(0);
}
