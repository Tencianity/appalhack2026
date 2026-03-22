#ifndef SCENE_H
#define SCENE_H

#include "scene/camera.h"
#include "scene/surface.h"
#include "scene/light.h"

#include <stdlib.h>
#include <stdatomic.h>

#define OBJECT_MAX 64
#define LIGHT_MAX 8
#define MAX_TILES 4096

typedef struct Scene Scene;
typedef struct Light Light;
typedef struct TileJob TileJob;

struct TileJob{
    Scene* scene;
    int startX, startY, endX, endY;
    uint32_t frameSeed;
};

struct Scene {
    int width;
    int height;
    uint32_t* buffer;
    Surface* objects[OBJECT_MAX];
    int objCount;
    Light* lights[LIGHT_MAX];
    int lightCount;
    Camera* cam;
    int tilesX;
    int tilesY;
    int frameSeed;
    BVHNode* bvhRoot;
    TileJob jobs[MAX_TILES];
    V3* accumBuffer;
    int sampleCount;
};


static inline void addSurface(Scene* scene, Surface* surface) {
    if (scene->objCount >= OBJECT_MAX) return;
    scene->objects[scene->objCount++] = surface;
}

static inline void addLight(Scene* scene, Light* light) {
    if (scene->lightCount >= LIGHT_MAX) return;
    scene->lights[scene->lightCount++] = light;
}

static inline void freeSurfaces(Scene* scene) {
    for (int i = 0; i < scene->objCount; i++) {
        scene->objects[i]->destroy(scene->objects[i]);
    }
}

static inline void freeLights(Scene* scene) {
    for (int i = 0; i < scene->lightCount; i++) {
        free(scene->lights[i]);
    }
}


#endif
