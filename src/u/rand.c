#include "rand.h"

static uint32_t s_state[1] = {1};

uint32_t xorshift32(uint32_t state[1]) {
    /* Algorithm "xor" from p. 4 of Marsaglia, "Xorshift RNGs" */
    uint32_t x = state[0];
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    return state[0] = x;
}

unsigned int rand() {
    return xorshift32(s_state);
}

