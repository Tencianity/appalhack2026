#ifndef CAMERA_H
#define CAMERA_H

#include "math/vector.h"

typedef struct {
    V3 origin;
    V3 lowerLeft;
    V3 horizontal;
    V3 vertical;     
} Camera;


Camera* createCameraLookAt(V3 origin, V3 target, V3 up, float aspect);


#endif
