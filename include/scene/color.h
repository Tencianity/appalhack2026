#ifndef COLOR_H
#define COLOR_H

#include "math/vector.h"

#include <stdint.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} RGBA;


static inline V3 rgbaToV3(RGBA rgba) {
    return (V3) {(float) rgba.r, (float) rgba.g, (float) rgba.b};
}

static inline RGBA v3ToRGBA(V3 v) {
    return (RGBA) {(int) v.x, (int) v.y, (int) v.z, 255};
}

#endif
