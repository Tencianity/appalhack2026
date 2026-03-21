#ifndef SCENE_H
#define SCENE_H

#include "scene/camera.h"
#include "scene/surface.h"

#include <stdlib.h>

typedef struct {
    int width;
    int height;
    uint32_t* buffer;
    Surface* objects[256];
    int objCount;
    Camera* cam;
} Scene;


static inline void freeSurfaces(Scene* scene) {
    for (int i = 0; i < scene->objCount; i++) {
        free(scene->objects[i]);
    }
}

#endif
