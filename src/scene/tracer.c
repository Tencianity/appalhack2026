#include "scene/tracer.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/ray.h"
#include "scene/hit.h"
#include "scene/color.h"

#include "math/random.h"

#include "util/thread.h"

#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>

#define AA_SAMPLES 4


void initTiles(Scene* scene) {
    scene->tilesX = (scene->width + TILE_SIZE - 1) / TILE_SIZE;
    scene->tilesY = (scene->height + TILE_SIZE - 1) / TILE_SIZE;
}


void renderTileJob(void* arg) {
    TileJob* job = (TileJob*) arg;
    Scene* scene = job->scene;
    float swDiv = (float) (scene->width - 1);
    float shDiv = (float) (scene->height - 1);
    for (int i = job->startY; i < job->endY; i++) {
        for (int j = job->startX; j < job->endX; j++) {
            V3 accumulation = {0, 0, 0};
            uint32_t seed = (j * 1973) + (i * 9277) + job->frameSeed;
            RNG rng;
            initRNG(&rng, seed);
            for (int s = 0; s < AA_SAMPLES; s++) {
                float du = rngFloat(&rng);
                float dv = rngFloat(&rng);
                float u = (j + du) / swDiv;
                float v = (i + dv) / shDiv;
                
                Ray ray = castRay(scene->cam, u, v);
                RGBA rgba = colorRay(ray, scene, &rng);
                accumulation.x += rgba.r;
                accumulation.y += rgba.g;
                accumulation.z += rgba.b;
            }

            accumulation.x /= AA_SAMPLES;
            accumulation.y /= AA_SAMPLES;
            accumulation.z /= AA_SAMPLES;

            uint32_t a = (uint32_t) 255;
            uint32_t r = (uint32_t) accumulation.x;
            uint32_t g = (uint32_t) accumulation.y;
            uint32_t b = (uint32_t) accumulation.z;

            uint32_t A = a << 24;
            uint32_t R = r << 16;
            uint32_t G = g << 8;
            uint32_t B = b;
            scene->buffer[i * scene->width + j] = A | R | G | B;
        }
    }
    free(job);
}


void renderScene(Scene* scene, ThreadPool* pool) {
    initTiles(scene);
    scene->frameSeed++;
    for (int ty = 0; ty < scene->tilesY; ty++) {
        for (int tx = 0; tx < scene->tilesX; tx++) {
            int startX = tx * TILE_SIZE;
            int startY = ty * TILE_SIZE;
            int endX = startX + TILE_SIZE;
            int endY = startY + TILE_SIZE;
            if (endX > scene->width) endX = scene->width;
            if (endY > scene->height) endY = scene->height;

            TileJob* job = malloc(sizeof(TileJob));
            job->scene = scene;
            job->startX = startX;
            job->startY = startY;
            job->endX = endX;
            job->endY = endY;
            job->frameSeed = scene->frameSeed;
            threadpoolAddJob(pool, renderTileJob, job);
        }
    }
    threadpoolWait(pool);
}


RGBA colorRay(Ray ray, Scene* scene, RNG* rng) {
    HitRec rec;
    if (hitScene(scene, ray, 0.001f, 1000.0f, &rec)) {
        V3 lightColor = {0, 0, 0};
        for (int i = 0; i < scene->lightCount; i++) {
            Light* light = scene->lights[i];
            V3 contribution = light->illuminate(light, scene, &rec, rng);
            lightColor = v3Add(lightColor, contribution);
        }

        V3 color = v3Mult(rec.mat.color, lightColor);
        color = v3Clamp(color, 0.0f, 1.0f);
        color.x = sqrtf(color.x);
        color.y = sqrtf(color.y);
        color.z = sqrtf(color.z);
        
        color = v3Scale(color, 255.0f);
        return (RGBA) {color.x, color.y, color.z, 255};
    }
    return (RGBA) {180, 180, 255, 255}; // Should be light blue if not hit
}


int hitScene(Scene* scene, Ray ray, float tMin, 
        float tMax, HitRec* rec) {
   
    HitRec temp;
    int hasHit = 0; // Boolean false
    float closest = tMax;

    for (int i = 0; i < scene->objCount; i++) {
        Surface* obj = scene->objects[i];
        if (obj->hit(obj, ray, tMin, closest, &temp)) {
            hasHit = 1; // Boolean true
            closest = temp.t;
            *rec = temp;
        }
    }
    return hasHit;
}
