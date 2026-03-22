#ifndef RANDOM_H
#define RANDOM_H

#include <stdint.h>

typedef struct {
    uint32_t state;
} RNG;


static inline void initRNG(RNG* rng, uint32_t seed) {
    rng->state = seed ? seed : 1;
}

static inline unsigned int rngNext(RNG* rng) {
    uint32_t x = rng->state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    rng->state = x;
    return x;
}

static inline float rngFloat(RNG* rng) {
    return (rngNext(rng) >> 8) * (1.0f / 16777216.0f);
}


#endif
