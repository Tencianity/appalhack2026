#ifndef TRACER_H
#define TRACER_H

#include "scene/camera.h"
#include "scene/ray.h"
#include "scene/color.h"
#include "scene/scene.h"

#include "util/thread.h"

#include <stdint.h>

#define TILE_SIZE 16

struct Scene;

void initTiles(Scene* scene);

void renderTileJob(void* arg);

void renderScene(Scene* scene, ThreadPool* pool);

Ray castRay(Camera* cam, float a, float b);

V3 colorRay(Ray ray, Scene* scene, RNG* rng);

int hitBVH(BVHNode* node, Ray ray, float tMin, float tMax, HitRec* rec);


#endif
