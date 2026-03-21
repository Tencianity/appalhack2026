#ifndef LIGHT_H
#define LIGHT_H

#include "math/vector.h"
#include "scene/scene.h"
#include "scene/surface.h"
#include "scene/camera.h"
#include "scene/ray.h"
#include "scene/hit.h"
#include "scene/color.h"

#define POINTLIGHT_TYPE 1
#define AMBIENTLIGHT_TYPE 2

typedef struct Light Light;
struct Light {
    int type;
    RGBA (*illuminate)(
        Light* self,
        Scene* scene,
        Surface* surface,
        HitRec* rec
    );
    V3 intensity;
};

typedef struct {
    Light base;
    V3 pos;
} PointLight;

typedef struct {
    Light base;
} AmbientLight;


PointLight createPointLight(V3 intensity, V3 pos);

AmbientLight createAmbientLight(V3 intensity);

RGBA pointIlluminate(Light* self, Scene* scene,
        Surface* surface, HitRec* rec);

RGBA ambientIlluminate(Light* self, Scene* scene,
        Surface* surface, HitRec* rec);


#endif
