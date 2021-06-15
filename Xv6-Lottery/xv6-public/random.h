#ifndef _RANDOM_H
#define _RANDOM_H

#include "types.h"

static uint state[3] = {1};

uint rand()
{
    uint x = state[0];
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    state[0] = x;
    x &= 65535;
    return x;
}

#endif