#ifndef SCENE_H
#define SCENE_H

#include "scene/camera.h"
#include "scene/surface.h"
#include "scene/light.h"

#include <stdlib.h>

typedef struct Light Light;
typedef struct Scene Scene;
struct Scene {
    int width;
    int height;
    uint32_t* buffer;
    Surface* objects[256];
    int objCount;
    Light* lights[32];
    int lightCount;
    Camera* cam;
};


static inline void freeSurfaces(Scene* scene) {
    for (int i = 0; i < scene->objCount; i++) {
        free(scene->objects[i]);
    }
}

static inline void freeLights(Scene* scene) {
    for (int i = 0; i < scene->lightCount; i++) {
        free(scene->lights[i]);
    }
}


#endif
