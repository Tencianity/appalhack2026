#include "scene/camera.h"
#include "math/vector.h"

#define VIEWPORT_HEIGHT 2.0f
#define FOCAL_LENGTH 1.0f

#include <stdlib.h>

Camera* createCamera(float aspect) {
    float viewportHeight = VIEWPORT_HEIGHT;
    float viewportWidth = aspect * viewportHeight;
    float focalLength = FOCAL_LENGTH;
    
    Camera* cam = (Camera*)malloc(sizeof(Camera));
    if (cam == NULL) return NULL;

    cam->origin = (V3) {0, 0, 0};
    cam->horizontal = (V3) {viewportWidth, 0, 0};
    cam->vertical = (V3) {0, viewportHeight, 0};
    cam->lowerLeft = (V3) {
        -viewportWidth / 2,
        -viewportHeight / 2,
        -focalLength
    };

    return cam;
}
