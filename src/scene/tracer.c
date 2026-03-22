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

#define AA_SAMPLES 2


void initTiles(Scene* scene) {
    scene->tilesX = (scene->width + TILE_SIZE - 1) / TILE_SIZE;
    scene->tilesY = (scene->height + TILE_SIZE - 1) / TILE_SIZE;
}


void renderTileJob(void* arg) {
    TileJob* job = (TileJob*) arg;
    Scene* scene = job->scene;
    float swDiv = (float)(scene->width - 1);
    float shDiv = (float)(scene->height - 1);
    for (int i = job->startY; i < job->endY; i++) {
        for (int j = job->startX; j < job->endX; j++) {
            uint32_t seed = (j * 1973) + (i * 9277) + job->frameSeed;
            RNG rng;
            initRNG(&rng, seed);
            float du = rngFloat(&rng);
            float dv = rngFloat(&rng);
            float u = (j + du) / swDiv;
            float v = (i + dv) / shDiv;
            Ray ray = castRay(scene->cam, u, v);
            V3 rgb = colorRay(ray, scene, &rng);
            int idx = i * scene->width + j;
            scene->accumBuffer[idx].x += rgb.x;
            scene->accumBuffer[idx].y += rgb.y;
            scene->accumBuffer[idx].z += rgb.z;
        }
    }
}


void renderScene(Scene* scene, ThreadPool* pool) {
    initTiles(scene);
    scene->frameSeed++;
    scene->sampleCount++;
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

    for (int i = 0; i < scene->width * scene->height; i++) {
        V3 c = scene->accumBuffer[i];
        c.x /= scene->sampleCount;
        c.y /= scene->sampleCount;
        c.z /= scene->sampleCount;
        c.x = sqrtf(c.x);
        c.y = sqrtf(c.y);
        c.z = sqrtf(c.z);

        if (c.x > 1.0f) c.x = 1.0f;
        if (c.y > 1.0f) c.y = 1.0f;
        if (c.z > 1.0f) c.z = 1.0f;
        c = v3Scale(c, 255.0f);

        scene->buffer[i] =
            (255 << 24) |
            ((uint32_t)c.x << 16) |
            ((uint32_t)c.y << 8) |
            ((uint32_t)c.z);
    }
}


V3 colorRay(Ray ray, Scene* scene, RNG* rng) {
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
        return v3Clamp(color, 0.0f, 1.0f);
    }

    return (V3) {0.8f, 0.8f, 1.0f};
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
