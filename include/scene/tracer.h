#ifndef TRACER_H
#define TRACER_H

#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/ray.h"
#include "scene/color.h"

#include "util/thread.h"

#include <stdint.h>

#define TILE_SIZE 16

typedef struct TileJob {
    Scene* scene;
    int startX, startY, endX, endY;
    uint32_t frameSeed;
} TileJob;


void initTiles(Scene* scene);

void renderTileJob(void* arg);

void renderScene(Scene* scene, ThreadPool* pool);

Ray castRay(Camera* cam, float a, float b);

RGBA colorRay(Ray ray, Scene* scene, RNG* rng);

int hitScene(Scene* scene, Ray ray, float tMin, 
        float tMax, HitRec* rec);

int hitShadow(Scene* scene, HitRec* rec, V3 dirNorm, float dirMag);


#endif
