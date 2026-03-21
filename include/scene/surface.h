#ifndef SURFACE_H
#define SURFACE_H

#include "math/vector.h"
#include "scene/ray.h"
#include "scene/hit.h"
#include "scene/material.h"

#define SPHERE_TYPE 1

typedef struct Surface Surface;
struct Surface {
    int type;
    int (*hit)(
        Surface* self,
        Ray ray,
        float tMin,
        float tMax,
        HitRec* rec
    );
    V3 velocity;
    Mat mat;
};

typedef struct {
    Surface base;
    V3 center;
    float radius;
} Sphere;


Sphere* createSphere(V3 center, float radius, Mat mat);

int hitSphere(Surface* self, Ray r, float tMin, 
        float tMax, HitRec* rec);


#endif
