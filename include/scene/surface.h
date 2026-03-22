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
    AABB bbox;
};

typedef struct BVHNode {
    AABB box;
    struct BVHNode* left;
    struct BVHNode* right;
    Surface** objects;
    int start;
    int end;
    int objectCount;
} BVHNode;

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


BVHNode* buildBVH(Surface** objects, int start, int end);
int hitAABB(AABB box, Ray ray, float tMin, float tMax);
AABB surroundAABB(AABB a, AABB b);
int compareBBoxAxis(const void* a, const void* b);

Sphere* createSphere(V3 center, float radius, Mat mat);
int hitSphere(Surface* self, Ray r, float tMin, float tMax, HitRec* rec);
void destroySphere(Surface* self);
AABB sphereAABB(Sphere* sphere);

Cube* createCube(V3 min, V3 max, Mat mat);
int hitCube(Surface* self, Ray r, float tMin, float tMax, HitRec* rec);
void destroyCube(Surface* self);
AABB cubeAABB(Cube* cube);

Quad* createQuad(V3 origin, V3 u, V3 v, Mat mat);
int hitQuad(Surface* self, Ray r, float tMin, float tMax, HitRec* rec);
void destroyQuad(Surface* self);
AABB quadAABB(Quad* quad);

Box* createBox(V3 min, V3 max, Mat mat);
int hitBox(Surface* self, Ray r, float tMin, float tMax, HitRec* rec);
void destroyBox(Surface* self);
AABB boxAABB(Box* box);

Plane* createPlane(float y, Mat mat);
int hitPlane(Surface* self, Ray r, float tMin, float tMax, HitRec* rec);
void destroyPlane(Surface* self);
AABB planeAABB(Plane* plane);


#endif
