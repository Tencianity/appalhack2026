#include "math/vector.h"
#include "scene/surface.h"
#include "scene/ray.h"
#include "scene/scene.h"
#include "scene/material.h"

#include <math.h>
#include <stdlib.h>


Sphere* createSphere(V3 center, float radius, Mat mat) {
    Sphere* s = (Sphere*)malloc(sizeof(Sphere));
    s->base.hit = hitSphere;
    s->base.type = SPHERE_TYPE;
    s->base.mat = mat;
    s->center = center;
    s->radius = radius;
    return s;
}

int hitSphere(Surface* self, Ray ray, float tMin, 
        float tMax, HitRec* rec) {

    Sphere* sphere = (Sphere*) self;

    // Quadratic formula
    V3 oc = v3Sub(ray.origin, sphere->center);
    float A = v3Dot(ray.direction, ray.direction);
    float halfB = v3Dot(oc, ray.direction);
    float C = v3Dot(oc, oc) - (sphere->radius * sphere->radius);
    float discrim = (halfB * halfB) - (A * C);

    if (discrim < 0) return 0;
    float sqrtd = sqrtf(discrim);

    float root = (-halfB - sqrtd) / A;
    if (root < tMin || root > tMax) {
        root = (-halfB + sqrtd) / A;
        if (root < tMin || root > tMax) return 0;
    }

    rec->t = root;
    rec->point = rayAt(ray, rec->t);
    rec->mat = sphere->base.mat;
    return 1;
}
