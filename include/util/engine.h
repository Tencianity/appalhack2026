#ifndef ENG_H
#define ENG_H

#include "scene/scene.h"
#include "scene/surface.h"

typedef struct {
    Quad faceFront;
    Quad faceBack;
    Quad faceLeft;
    Quad faceRight;
    Quad faceTop;
    Quad faceBottom;
} myCube;

void transpose(Scene* scene, Surface* object, float speed);
void mvSphere(Scene* scene, Sphere* sphere, float speed);

#endif