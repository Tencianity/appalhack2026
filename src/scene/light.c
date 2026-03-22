#include "scene/light.h"
#include "scene/tracer.h"
#include "scene/surface.h"
#include "scene/hit.h"
#include "math/vector.h"
#include "scene/color.h"

#include <math.h>


PointLight* createPointLight(V3 intensity, V3 pos) {
    PointLight* light = (PointLight*)malloc(sizeof(PointLight));
    light->base.type = POINTLIGHT_TYPE;
    light->base.illuminate = pointIlluminate;
    light->base.intensity = intensity;
    light->pos = pos;

    return light;
}


AmbientLight* createAmbientLight(V3 intensity) {
    AmbientLight* light = (AmbientLight*)malloc(sizeof(AmbientLight));
    light->base.type = AMBIENTLIGHT_TYPE;
    light->base.illuminate = ambientIlluminate;
    light->base.intensity = intensity;

    return light;
}


V3 pointIlluminate(Light* self, Scene* scene, HitRec* rec) {
    
    PointLight* light = (PointLight*) self;
    V3 lightDir = v3Sub(light->pos, rec->point);
    float lightDirMag = v3Norm(lightDir);
    V3 lightDirNorm = v3Normalize(lightDir);

    if (hitShadow(scene, rec, lightDirNorm, lightDirMag)) return (V3) {0, 0, 0};

    V3 e = v3InvScale(light->base.intensity, lightDirMag * lightDirMag);
    float NdotL = fmaxf(0.0f, v3Dot(rec->normal, lightDirNorm));
    V3 color = rec->mat.color;
    return v3Scale(v3Mult(color, e), NdotL);
}


V3 ambientIlluminate(Light* self, Scene* scene, HitRec* rec) {

}
