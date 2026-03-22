#include "math/vector.h"
#include "scene/surface.h"
#include "scene/ray.h"
#include "scene/scene.h"
#include "scene/material.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>


Sphere* createSphere(V3 center, float radius, Mat mat) {
    Sphere* s = (Sphere*) malloc(sizeof(Sphere));
    s->base.type = SPHERE_TYPE;
    s->base.hit = hitSphere;
    s->base.destroy = destroySphere;
    s->base.mat = mat;
    s->center = center;
    s->radius = radius;
    return s;
}


int hitSphere(Surface* self, Ray ray, float tMin, 
        float tMax, HitRec* rec) {

    Sphere* sphere = (Sphere*) self;
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
    rec->normal = v3Normalize(v3Sub(rec->point, sphere->center));
    rec->mat = sphere->base.mat;
    return 1;
}


void destroySphere(Surface* self) {
    free(self);
}


Cube* createCube(V3 min, V3 max, Mat mat) {
    Cube* c = (Cube*) malloc(sizeof(Cube));
    c->base.type = CUBE_TYPE;
    c->base.hit = hitCube;
    c->base.destroy = destroyCube;
    c->base.mat = mat;
    c->base.velocity = (V3) {0,0,0};
    c->min = min;
    c->max = max;
    return c;
}


int hitCube(Surface* self, Ray r, float tMin, float tMax, HitRec* rec) {
    Cube* cube = (Cube*)self;
    float t0 = tMin;
    float t1 = tMax;
    for (int i = 0; i < 3; i++) {
        float invD = 1.0f / ((float*) &r.direction)[i];
        float tNear = (((float*) &cube->min)[i] - ((float*) &r.origin)[i]) * invD;
        float tFar  = (((float*) &cube->max)[i] - ((float*) &r.origin)[i]) * invD;
        if (invD < 0.0f) {
            float temp = tNear;
            tNear = tFar;
            tFar = temp;
        }

        if (tNear > t0) t0 = tNear;
        if (tFar < t1) t1 = tFar;
        if (t0 > t1) return 0;
    }

    rec->t = t0;
    rec->point = v3Add(r.origin, v3Scale(r.direction, t0));
    V3 p = rec->point;
    float epsilon = 0.0001f;

    if (fabsf(p.x - cube->min.x) < epsilon) 
        rec->normal = (V3) {-1,0,0};
    else if (fabsf(p.x - cube->max.x) < epsilon) 
        rec->normal = (V3) {1,0,0};
    else if (fabsf(p.y - cube->min.y) < epsilon) 
        rec->normal = (V3) {0,-1,0};
    else if (fabsf(p.y - cube->max.y) < epsilon) 
        rec->normal = (V3) {0,1,0};
    else if (fabsf(p.z - cube->min.z) < epsilon) 
        rec->normal = (V3) {0,0,-1};
    else 
        rec->normal = (V3) {0,0,1};

    rec->mat = cube->base.mat;
    return 1;
}


void destroyCube(Surface* self) {
    free(self);
}


Quad* createQuad(V3 origin, V3 u, V3 v, Mat mat) {
    Quad* q = (Quad*) malloc(sizeof(Quad));
    q->base.type = QUAD_TYPE;
    q->base.hit = hitQuad;
    q->base.destroy = destroyQuad;
    q->base.mat = mat;
    q->origin = origin;
    q->u = u;
    q->v = v;
    q->normal = v3Normalize(v3Cross(u, v));
    return q;
}


int hitQuad(Surface* self, Ray r, float tMin, float tMax, HitRec* rec) {
    Quad* q = (Quad*) self;
    float denom = v3Dot(q->normal, r.direction);
    if (fabsf(denom) < 0.00001f) return 0;
    float t = v3Dot(v3Sub(q->origin, r.origin), q->normal) / denom;
    if (t < tMin || t > tMax) return 0;

    V3 p = v3Add(r.origin, v3Scale(r.direction, t));
    V3 rel = v3Sub(p, q->origin);
    float uLen = v3Dot(q->u, q->u);
    float vLen = v3Dot(q->v, q->v);
    float uCoord = v3Dot(rel, q->u) / uLen;
    float vCoord = v3Dot(rel, q->v) / vLen;

    if (uCoord < 0 || uCoord > 1 || vCoord < 0 || vCoord > 1)
        return 0;

    rec->t = t;
    rec->point = p;
    rec->normal = q->normal;
    rec->mat = q->base.mat;
    return 1;
}


void destroyQuad(Surface* self) {
    free(self);
}


Box* createBox(V3 min, V3 max, Mat mat) {
    Box* b = (Box*) malloc(sizeof(Box));
    b->base.type = BOX_TYPE;
    b->base.hit = hitBox;
    b->base.destroy = destroyBox;
    b->base.mat = mat;

    V3 dx = {max.x - min.x, 0, 0};
    V3 dy = {0, max.y - min.y, 0};
    V3 dz = {0, 0, max.z - min.z};

    // Front
    b->faces[0] = createQuad(
        (V3) {min.x, min.y, max.z}, 
        dx, 
        dy, 
        mat
    );
    // Back
    b->faces[1] = createQuad(
        (V3) {max.x, min.y, min.z}, 
        v3Negate(dx), 
        dy, 
        mat
    );
    // Left
    b->faces[2] = createQuad(
        (V3) {min.x, min.y, min.z}, 
        dz, 
        dy, 
        mat
    );
    // Right
    b->faces[3] = createQuad(
        (V3) {max.x, min.y, max.z}, 
        v3Negate(dz), 
        dy, 
        mat
    );
    // Top
    b->faces[4] = createQuad(
        (V3) {min.x, max.y, max.z}, 
        dx, 
        v3Negate(dz), 
        mat
    );
    // Bottom
    b->faces[5] = createQuad(
        (V3) {min.x, min.y, min.z}, 
        dx, 
        dz, 
        mat
    );

    return b;
}


int hitBox(Surface* self, Ray r, float tMin, float tMax, HitRec* rec) {
    Box* box = (Box*) self;
    HitRec tempRec;
    int hitAnything = 0;
    float closest = tMax;
    for (int i = 0; i < 6; i++) {
        if (box->faces[i]->base.hit((Surface*) box->faces[i], 
                    r, tMin, closest, &tempRec)) {

            hitAnything = 1;
            closest = tempRec.t;
            *rec = tempRec;
        }
    }
    return hitAnything;
}


void destroyBox(Surface* self) {
    Box* box = (Box*) self;
    for (int i = 0; i < 6; i++) {
        destroyQuad((Surface*) box->faces[i]);
    }
    free(box);
}


Plane* createPlane(float y, Mat mat) {
    Plane* p = (Plane*) malloc(sizeof(Plane));
    p->base.hit = hitPlane;
    p->base.destroy = destroyPlane;
    p->base.mat = mat;
    p->y = y;
    return p;
}


int hitPlane(Surface* self, Ray r, float tMin, float tMax, HitRec* rec) {
    Plane* plane = (Plane*)self;
    float denom = r.direction.y;
    if (fabs(denom) < 1e-6f) return 0;
    float t = (plane->y - r.origin.y) / denom;
    if (t < tMin || t > tMax) return 0;

    rec->t = t;
    rec->point = v3Add(r.origin, v3Scale(r.direction, t));
    rec->normal = (V3){0, 1, 0};
    rec->mat = self->mat;
    return 1;
}


void destroyPlane(Surface* self) {
    free(self);
}
