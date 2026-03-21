#ifndef HIT_H
#define HIT_H

#include "math/vector.h"
#include "scene/material.h"

typedef struct {
    V3 point;
    V3 normal;
    float t;
    Mat mat;
} HitRec;

#endif
