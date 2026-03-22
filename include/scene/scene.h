#ifndef SCENE_H
#define SCENE_H

#include "scene/camera.h"
#include "scene/surface.h"
#include "scene/light.h"

#include <stdlib.h>
#include <stdatomic.h>

typedef struct Light Light;
typedef struct Scene Scene;
struct Scene {
    V3 origin;
    int width;
    int height;
    uint32_t* buffer;
    Surface* objects[256];
    int objCount;
    Light* lights[32];
    int lightCount;
    Camera* cam;
    int tilesX;
    int tilesY;
    int frameSeed;
};


static inline void addSurface(Scene* scene, Surface* surface) {
    scene->objects[scene->objCount++] = surface;
}

static inline void addLight(Scene* scene, Light* light) {
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
