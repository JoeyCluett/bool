#pragma once

#include <sys/time.h>

uint64_t UsecTimestamp(void) {
    timeval tv;
    gettimeofday(&tv, NULL);

    return (tv.tv_sec * 1000000) + tv.tv_usec;
}
