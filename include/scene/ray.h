#ifndef RAY_H
#define RAY_H

#include "math/vector.h"
#include "scene/camera.h"

typedef struct {
    V3 origin;
    V3 direction;
} Ray;


static inline V3 rayAt(Ray ray, float t) {
    return v3Add(ray.origin, v3Scale(ray.direction, t));
}

Ray castRay(Camera* cam, float a, float b);


#endif
