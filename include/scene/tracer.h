#ifndef TRACER_H
#define TRACER_H

#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/ray.h"
#include "scene/color.h"

#include <stdint.h>

typedef struct {
    Scene* scene;
    int startRow;
    int endRow;
    int frameSeed;
} RenderTask;


void* renderThread(void* arg);

void renderScene(Scene* scene, int frameSeed);

Ray castRay(Camera* cam, float a, float b);

RGBA colorRay(Ray ray, Scene* scene, RNG* rng);

int hitScene(Scene* scene, Ray ray, float tMin, 
        float tMax, HitRec* rec);

int hitShadow(Scene* scene, HitRec* rec, V3 dirNorm, float dirMag);


#endif
