#ifndef SURFACE_H
#define SURFACE_H

#include "math/vector.h"
#include "scene/ray.h"
#include "scene/hit.h"
#include "scene/material.h"

#define SPHERE_TYPE 1
#define CUBE_TYPE 2
#define QUAD_TYPE 3
#define BOX_TYPE 4

typedef struct {
    V3 min;
    V3 max;
} AABB;

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
    void (*destroy)(
        Surface* self
    );
    V3 velocity;
    Mat mat;
};

typedef struct {
    Surface base;
    V3 center;
    float radius;
} Sphere;

typedef struct {
    Surface base;
    V3 min;
    V3 max;
} Cube;

typedef struct {
    Surface base;
    V3 origin;
    V3 u;
    V3 v;
    V3 normal;
} Quad;

typedef struct {
    Surface base;
    Quad* faces[6];
} Box;

typedef struct {
    Surface base;
    float y;
} Plane;


Sphere* createSphere(V3 center, float radius, Mat mat);
int hitSphere(Surface* self, Ray r, float tMin, float tMax, HitRec* rec);
void destroySphere(Surface* self);

Cube* createCube(V3 min, V3 max, Mat mat);
int hitCube(Surface* self, Ray r, float tMin, float tMax, HitRec* rec);
void destroyCube(Surface* self);

Quad* createQuad(V3 origin, V3 u, V3 v, Mat mat);
int hitQuad(Surface* self, Ray r, float tMin, float tMax, HitRec* rec);
void destroyQuad(Surface* self);

Box* createBox(V3 min, V3 max, Mat mat);
int hitBox(Surface* self, Ray r, float tMin, float tMax, HitRec* rec);
void destroyBox(Surface* self);

Plane* createPlane(float y, Mat mat);
int hitPlane(Surface* self, Ray r, float tMin, float tMax, HitRec* rec);
void destroyPlane(Surface* self);


#endif
