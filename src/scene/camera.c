#include "scene/camera.h"
#include "math/vector.h"

#define VIEWPORT_HEIGHT 2.0f
#define FOCAL_LENGTH 1.0f

#include <stdlib.h>


Camera* createCameraFPS(V3 origin, float aspect) {
    Camera* cam = malloc(sizeof(Camera));
    cam->origin = origin;
    cam->yaw = 0.0f;
    cam->pitch = 0.0f;
    updateCamera(cam, aspect);
    return cam;
}


void updateCamera(Camera* cam, float aspect) {
    float cosPitch = cosf(cam->pitch);
    float sinPitch = sinf(cam->pitch);
    float cosYaw = cosf(cam->yaw);
    float sinYaw = sinf(cam->yaw);

    V3 forward = {
        cosPitch * cosYaw,
        sinPitch,
        cosPitch * sinYaw
    };


    cam->forward = v3Normalize(forward);
    V3 worldUp = {0, 1, 0};
    cam->right = v3Normalize(v3Cross(cam->forward, worldUp));
    cam->up = v3Cross(cam->forward, cam->right);
    float viewportHeight = VIEWPORT_HEIGHT;
    float viewportWidth = aspect * viewportHeight;
    float focalLength = FOCAL_LENGTH;
    cam->horizontal = v3Scale(cam->right, viewportWidth);
    cam->vertical = v3Scale(cam->up, viewportHeight);
    cam->lowerLeft =
        v3Sub(
            v3Sub(
                v3Sub(cam->origin,
                      v3Scale(cam->horizontal, 0.5f)),
                v3Scale(cam->vertical, 0.5f)),
            v3Scale(cam->forward, focalLength)
        );
}
