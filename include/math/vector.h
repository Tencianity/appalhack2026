#ifndef VECTOR_H
#define VECTOR_H

#include "math/random.h"

#include <math.h>

typedef struct {
    float x;
    float y;
    float z;
} V3;


static inline V3 v3Negate(V3 v) {
    return (V3) {-v.x, -v.y, -v.z};
}

static inline float v3Norm(V3 v) {
    return sqrtf((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

static inline float v3Norm2(V3 v) {
    return (v.x * v.x) + (v.y * v.y) + (v.z * v.z);
}

static inline V3 v3Normalize(V3 v) {
    float n = v3Norm(v);
    if (n == 0.0f) return (V3) {0, 0, 0};
    else return (V3) {v.x / n, v.y / n, v.z / n};
}

static inline V3 v3Min(V3 u, V3 v) {
    return (V3) {
        fminf(u.x, v.x),
        fminf(u.y, v.y),
        fminf(u.z, v.z)
    };
}

static inline V3 v3Max(V3 u, V3 v) {
    return (V3) {
        fmaxf(u.x, v.x),
        fmaxf(u.y, v.y),
        fmaxf(u.z, v.z)
    };
}

static inline V3 v3Clamp(V3 v, float min, float max) {
    return (V3) {
        fminf(fmaxf(v.x, min), max),
        fminf(fmaxf(v.y, min), max),
        fminf(fmaxf(v.x, min), max)
    };
}

static inline V3 v3Add(V3 u, V3 v) {
    return (V3) {u.x + v.x, u.y + v.y, u.z + v.z};
}

static inline V3 v3Sub(V3 u, V3 v) {
    return (V3) {u.x - v.x, u.y - v.y, u.z - v.z};
}

static inline V3 v3Mult(V3 u, V3 v) {
    return (V3) {u.x * v.x, u.y * v.y, u.z * v.z};
}

static inline float v3Dot(V3 u, V3 v) {
    return (u.x * v.x) + (u.y * v.y) + (u.z * v.z);
}

static inline V3 v3Cross(V3 u, V3 v) {
    return (V3) {
        (u.y * v.z) - (u.z * v.y),
        (u.z * v.x) - (u.x * v.z),
        (u.x * v.y) - (u.y * v.x)
    };
}

static inline V3 v3Scale(V3 v, float s) {
    return (V3) {v.x * s, v.y * s, v.z * s};
}

static inline V3 v3InvScale(V3 v, float s) {
    return (V3) {v.x / s, v.y / s, v.z / s};
}

static inline V3 v3Reflect(V3 v, V3 n) {
    float d = v3Dot(v, n);
    return v3Sub(v, v3Scale(n, 2.0f * d));
}

static inline float v3Dist(V3 u, V3 v) {
    return v3Norm(v3Sub(u, v));
}

static inline V3 v3Lerp(V3 u, V3 v, float t) {
    return v3Add(u, v3Scale(v3Sub(u, v), t));
}

static inline V3 v3Project(V3 u, V3 v) {
    float d = v3Dot(u, v) / v3Dot(v, v);
    return v3Scale(v, d);
}

static inline V3 randomOffset(RNG* rng, float radius) {
    return (V3) {
        (rngFloat(rng) - 0.5f) * 2.0f * radius,
        (rngFloat(rng) - 0.5f) * 2.0f * radius,
        (rngFloat(rng) - 0.5f) * 2.0f * radius
    };
}


#endif
