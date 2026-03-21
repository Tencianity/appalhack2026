#include "scene/light.h"
#include "scene/tracer.h"
#include "scene/camera.h"
#include "scene/ray.h"
#include "scene/hit.h"
#include "math/vector.h"
#include "scene/color.h"

#include <math.h>


PointLight createPointLight(V3 intensity, V3 pos) {
    PointLight light;
    light.base.type = POINTLIGHT_TYPE;
    light.base.illuminate = pointIlluminate;
    light.base.intensity = intensity;
    light.pos = pos;

    return light;
}


AmbientLight createAmbientLight(V3 intensity) {
    AmbientLight light;
    light.base.type = AMBIENTLIGHT_TYPE;
    light.base.illuminate = ambientIlluminate;
    light.base.intensity = intensity;

    return light;
}


RGBA pointIlluminate(Light* self, Scene* scene,
        Surface* surface, HitRec* rec) {
    
    PointLight* light = (PointLight*) self;

    V3 lightDir = v3Sub(light->pos, rec->point);
    float lightDirMag = v3Norm(lightDir);
    V3 lightDirNorm = v3Normalize(lightDir);

    V3 shadowOrig = v3Add(rec->point, v3Scale(lightDirNorm, 0.00001f));
    Ray shadowRay = (Ray){shadowOrig, lightDirNorm};
    HitRec shadowRec;

    int shadowHit = hitScene(scene, shadowRay, 
            0.001f, lightDirMag, &shadowRec);
    if (shadowHit) return (RGBA){0, 0, 0};

    V3 e = v3InvScale(light->base.intensity, lightDirMag * lightDirMag);
    float NdotL = fmaxf(0.0f, v3Dot(rec->normal, lightDirNorm));
    V3 color = rgbaToV3(rec->mat.color);
    RGBA rgba = v3ToRGBA(v3Scale(v3Mult(color, e), NdotL));
    return rgba;
}


RGBA ambientIlluminate(Light* self, Scene* scene, 
        Surface* surface, HitRec* rec) {

}
