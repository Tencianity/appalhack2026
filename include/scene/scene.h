#ifndef SCENE_H
#define SCENE_H

#include "scene/camera.h"
#include "scene/surface.h"

typedef struct {
    int width;
    int height;
    uint32_t* buffer;
    Surface* objects[256];
    int objCount;
    Camera* cam;
} Scene;


#endif
