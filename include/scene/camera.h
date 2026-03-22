#ifndef CAMERA_H
#define CAMERA_H

#include "math/vector.h"

typedef struct {
    V3 origin;
    V3 lowerLeft;
    V3 horizontal;
    V3 vertical;
    V3 forward;
    V3 right;
    V3 up;
    float yaw;
    float pitch;
} Camera;


Camera* createCameraFPS(V3 origin, float aspect);
void updateCamera(Camera* cam, float aspect);


#endif
