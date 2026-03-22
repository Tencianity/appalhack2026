#include "scene/camera.h"
#include "math/vector.h"

#define VIEWPORT_HEIGHT 2.0f
#define FOCAL_LENGTH 1.0f

#include <stdlib.h>

Camera* createCameraLookAt(V3 origin, V3 target, V3 up, float aspect) {
    Camera* cam = malloc(sizeof(Camera));
    V3 forward = v3Normalize(v3Sub(target, origin));
    V3 right = v3Normalize(v3Cross(forward, up));
    V3 trueUp = v3Cross(right, forward);
    float viewportHeight = VIEWPORT_HEIGHT;
    float viewportWidth  = aspect * viewportHeight;
    float focalLength    = FOCAL_LENGTH;

    cam->origin = origin;
    cam->horizontal = v3Scale(right, viewportWidth);
    cam->vertical   = v3Scale(trueUp, -viewportHeight);
    cam->lowerLeft =
        v3Sub(
            v3Sub(
                v3Sub(origin, v3Scale(cam->horizontal, 0.5f)),
                v3Scale(cam->vertical, 0.5f)
            ),
            v3Scale(forward, focalLength)
        );

    return cam;
}
