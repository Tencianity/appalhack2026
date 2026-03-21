#include "scene/light.h"
#include "scene/ray.h"
#include "scene/hit.h"
#include "math/vector.h"


PointLight createPointLight(V3 intensity, V3 pos) {
    PointLight light;
    light.base.type = POINTLIGHT_TYPE;
    light.base.illuminate = pointIlluminate;
    light.base.intensity = intensity;
    light.pos = pos;

    return light;
}


AmbientLight createAmbientLight(V3 intensity) {
    PointLight light;
    light.base.type = AMBIENTLIGHT_TYPE;
    light.base.illuminate = ambientIlluminate;
    light.base.intensity = intensity;

    return light;
}


V3 pointIlluminate(Light* self, Ray ray, HitRec* rec) {
    PointLight* light = (PointLight*) self;
    V3 r = v3Sub(light->pos, rec->point);
    float rMag = v3Norm(r);
    V3 rNorm = v3Normalize(r);

    Ray shadowRay = castRay()
}


V3 ambientIlluminate(Light* self, Ray ray, HitRec* rec) {

}
