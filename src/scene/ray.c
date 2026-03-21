#include "scene/ray.h"
#include "scene/camera.h"
#include "math/vector.h"

Ray castRay(Camera* cam, float a, float b) {
    V3 dir = v3Sub(
        v3Add(
            v3Add(cam->lowerLeft, v3Scale(cam->horizontal, a)),
            v3Scale(cam->vertical, b)
        ),
        cam->origin
    );

    return (Ray) {cam->origin, dir};
}
