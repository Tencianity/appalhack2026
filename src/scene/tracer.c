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

#define AA_SAMPLES 8


void initTiles(Scene* scene) {
    scene->tilesX = (scene->width + TILE_SIZE - 1) / TILE_SIZE;
    scene->tilesY = (scene->height + TILE_SIZE - 1) / TILE_SIZE;
}


void renderTileJob(void* arg) {
    TileJob* job = (TileJob*) arg;
    Scene* scene = job->scene;
    float invW = 1.0f / (scene->width - 1);
    float invH = 1.0f / (scene->height - 1);
    for (int i = job->startY; i < job->endY; i++) {
        for (int j = job->startX; j < job->endX; j++) {
            V3 accumulation = {0, 0, 0};
            uint32_t seed = (j * 1973) + (i * 9277) + job->frameSeed;
            RNG rng;
            initRNG(&rng, seed);
            for (int s = 0; s < AA_SAMPLES; s++) {
                float du = rngFloat(&rng);
                float dv = rngFloat(&rng);
                float u = (j + du) * invW;
                float v = (i + dv) * invH;

                Ray ray = castRay(scene->cam, u, v);
                RGBA rgba = colorRay(ray, scene, &rng);
                accumulation.x += rgba.r;
                accumulation.y += rgba.g;
                accumulation.z += rgba.b;
            }
            accumulation.x /= AA_SAMPLES;
            accumulation.y /= AA_SAMPLES;
            accumulation.z /= AA_SAMPLES;
            uint32_t r = (uint32_t) accumulation.x;
            uint32_t g = (uint32_t) accumulation.y;
            uint32_t b = (uint32_t) accumulation.z;
            scene->buffer[i * scene->width + j] =
                (255 << 24) | (r << 16) | (g << 8) | b;
        }
    }
}


void renderScene(Scene* scene, ThreadPool* pool) {
    initTiles(scene);
    scene->frameSeed++;
    int jobCount = 0;
    for (int ty = 0; ty < scene->tilesY; ty++) {
        for (int tx = 0; tx < scene->tilesX; tx++) {
            TileJob* job = &scene->jobs[jobCount++];
            int startX = tx * TILE_SIZE;
            int startY = ty * TILE_SIZE;
            int endX = startX + TILE_SIZE;
            int endY = startY + TILE_SIZE;
            if (endX > scene->width)  endX = scene->width;
            if (endY > scene->height) endY = scene->height;
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
    if (hitBVH(scene->bvhRoot, ray, 0.001f, 1000.0f, &rec)) {
        V3 baseColor = rec.mat.color;
        if (rec.mat.isGround) {
            V3 p = rec.point;
            float gridSize = 0.25f;
            int x = (int) floorf(p.x / gridSize);
            int z = (int) floorf(p.z / gridSize);
            int checker = (x + z) & 1;
            V3 c1 = {0.4f, 0.4f, 0.8f};
            V3 c2 = {0.2f, 0.2f, 0.8f};
            V3 gridColor = checker ? c1 : c2;
            float lineWidth = 0.05f;
            float fx = fabsf(p.x / gridSize - roundf(p.x / gridSize));
            float fz = fabsf(p.z / gridSize - roundf(p.z / gridSize));

            if (fx < lineWidth || fz < lineWidth) {
                gridColor = (V3) {0.0f, 0.0f, 0.0f};
            }
            baseColor = gridColor;
        }
        V3 color = {0, 0, 0};
        for (int i = 0; i < scene->lightCount; i++) {
            Light* light = scene->lights[i];
            V3 lightColor = light->illuminate(light, scene, &rec, rng);
            color = v3Add(color, v3Mult(baseColor, lightColor));
        }

        color = v3Clamp(color, 0.0f, 1.0f);
        color.x = powf(color.x, 0.5f);
        color.y = powf(color.y, 0.5f);
        color.z = powf(color.z, 0.5f);
        color = v3Scale(color, 255.0f);
        return (RGBA) {color.x, color.y, color.z, 255};
    }

    return (RGBA){180, 180, 255, 255};
}


int hitBVH(BVHNode* node, Ray ray, float tMin, float tMax, HitRec* rec) {
    if (!hitAABB(node->box, ray, tMin, tMax))
        return 0;

    HitRec tempRec;
    int hitAnything = 0;
    float closest = tMax;

    if (node->left == NULL && node->right == NULL) {
        for (int i = node->start; i < node->end; i++) {
            if (node->objects[i]->hit(node->objects[i], ray, tMin, closest, &tempRec)) {
                hitAnything = 1;
                closest = tempRec.t;
                *rec = tempRec;
            }
        }
        return hitAnything;
    }

    if (node->left)
        if (hitBVH(node->left, ray, tMin, closest, &tempRec)) {
            hitAnything = 1;
            closest = tempRec.t;
            *rec = tempRec;
        }

    if (node->right)
        if (hitBVH(node->right, ray, tMin, closest, &tempRec)) {
            hitAnything = 1;
            *rec = tempRec;
        }

    return hitAnything;
}
