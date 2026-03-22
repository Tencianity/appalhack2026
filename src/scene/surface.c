#include "math/vector.h"
#include "scene/surface.h"
#include "scene/ray.h"
#include "scene/scene.h"
#include "scene/material.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>


BVHNode* buildBVH(Surface** objects, int start, int end) {
    BVHNode* node = malloc(sizeof(BVHNode));
    int count = end - start;
    AABB bbox = objects[start]->bbox;
    for (int i = start + 1; i < end; i++) {
        bbox = surroundAABB(bbox, objects[i]->bbox);
    }
    node->box = bbox;
    node->objects = objects;
    node->start = start;
    node->end = end;
    node->objectCount = count;

    if (count <= 2) {
        node->left = NULL;
        node->right = NULL;
        return node;
    }

    int axis = 0;
    qsort(objects + start, count, sizeof(Surface*), compareBBoxAxis);
    int mid = start + count / 2;
    node->left  = buildBVH(objects, start, mid);
    node->right = buildBVH(objects, mid, end);
    return node;
}


int hitAABB(AABB box, Ray ray, float tMin, float tMax) {
    for (int i = 0; i < 3; i++) {
        float origin = ((float*)&ray.origin)[i];
        float direction = ((float*)&ray.direction)[i];
        float minB = ((float*)&box.min)[i];
        float maxB = ((float*)&box.max)[i];

        float invD = 1.0f / direction;

        float t0 = (minB - origin) * invD;
        float t1 = (maxB - origin) * invD;

        if (invD < 0.0f) {
            float temp = t0;
            t0 = t1;
            t1 = temp;
        }

        if (t0 > tMin) tMin = t0;
        if (t1 < tMax) tMax = t1;

        if (tMax <= tMin)
            return 0;
    }

    return 1;
}


AABB surroundAABB(AABB a, AABB b) {
    AABB result;
    result.min.x = fminf(a.min.x, b.min.x);
    result.min.y = fminf(a.min.y, b.min.y);
    result.min.z = fminf(a.min.z, b.min.z);
    result.max.x = fmaxf(a.max.x, b.max.x);
    result.max.y = fmaxf(a.max.y, b.max.y);
    result.max.z = fmaxf(a.max.z, b.max.z);
    return result;
}


int compareBBoxAxis(const void* a, const void* b) {
    Surface* sa = *(Surface**) a;
    Surface* sb = *(Surface**) b;
    float ca = sa->bbox.min.x;
    float cb = sb->bbox.min.x;
    if (ca < cb) return -1;
    if (ca > cb) return 1;
    return 0;
}


Sphere* createSphere(V3 center, float radius, Mat mat) {
    Sphere* s = (Sphere*) malloc(sizeof(Sphere));
    s->base.type = SPHERE_TYPE;
    s->base.hit = hitSphere;
    s->base.destroy = destroySphere;
    s->base.mat = mat;
    s->center = center;
    s->radius = radius;
    
    s->base.bbox = sphereAABB(s);
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


AABB sphereAABB(Sphere* sphere) {
    AABB box;
    V3 r = (V3){sphere->radius, sphere->radius, sphere->radius};
    box.min = v3Sub(sphere->center, r);
    box.max = v3Add(sphere->center, r);
    return box;
}


Cube* createCube(V3 min, V3 max, Mat mat) {
    Cube* c = (Cube*) malloc(sizeof(Cube));
    c->base.type = CUBE_TYPE;
    c->base.hit = hitCube;
    c->base.destroy = destroyCube;
    c->base.bbox = cubeAABB(c);
    c->base.mat = mat;
    c->base.velocity = (V3) {0,0,0};
    c->min = min;
    c->max = max;
    return c;
}


AABB cubeAABB(Cube* cube) {
    AABB box;
    box.min = cube->min;
    box.max = cube->max;
    return box;
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
    
    q->base.bbox = quadAABB(q);
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


AABB quadAABB(Quad* quad) {
    V3 p0 = quad->origin;
    V3 p1 = v3Add(quad->origin, quad->u);
    V3 p2 = v3Add(quad->origin, quad->v);
    V3 p3 = v3Add(p1, quad->v);
    V3 min = p0;
    V3 max = p0;
    V3 points[4] = {p0, p1, p2, p3};

    for (int i = 0; i < 4; i++) {
        min.x = fminf(min.x, points[i].x);
        min.y = fminf(min.y, points[i].y);
        min.z = fminf(min.z, points[i].z);

        max.x = fmaxf(max.x, points[i].x);
        max.y = fmaxf(max.y, points[i].y);
        max.z = fmaxf(max.z, points[i].z);
    }

    AABB box;
    box.min = min;
    box.max = max;
    return box;
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

    b->base.bbox = boxAABB(b);
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


AABB boxAABB(Box* b) {
    AABB box;
    box = b->faces[0]->base.bbox;
    for (int i = 1; i < 6; i++) {
        box = surroundAABB(box, b->faces[i]->base.bbox);
    }
    return box;
}


Plane* createPlane(float y, Mat mat) {
    Plane* p = (Plane*) malloc(sizeof(Plane));
    p->base.hit = hitPlane;
    p->base.destroy = destroyPlane;
    p->base.mat = mat;
    p->y = y;
    p->base.bbox = planeAABB(p);
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


AABB planeAABB(Plane* plane) {
    float extent = 50.0f;
    AABB box;
    box.min = (V3) {-extent, plane->y - 0.01f, -extent};
    box.max = (V3) { extent, plane->y + 0.01f,  extent};
    return box;
}
