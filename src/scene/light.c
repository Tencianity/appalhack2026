#include "scene/light.h"
#include "scene/tracer.h"
#include "scene/surface.h"
#include "scene/hit.h"
#include "scene/color.h"

#include "math/vector.h"
#include "math/random.h"

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


V3 pointIlluminate(Light* self, Scene* scene, HitRec* rec, RNG* rng) {
    PointLight* light = (PointLight*) self;
    V3 lightDir = v3Sub(light->pos, rec->point);
    float lightDirMag = v3Norm(lightDir);
    V3 lightDirNorm = v3Normalize(lightDir);

    int samples = 8;
    float visibility = 0.0f;
    for (int i = 0; i < samples; i++) {
        // Jitter light position slightly
        V3 jitter = randomOffset(rng, 0.01f);
        V3 samplePos = v3Add(light->pos, jitter);

        V3 dir = v3Sub(samplePos, rec->point);
        float dist = v3Norm(dir) - 0.001f;
        if (dist < 0) dist = 0;

        V3 dirNorm = v3Normalize(dir);
        V3 orig = v3Add(rec->point, v3Scale(rec->normal, 0.001f));
        Ray shadowRay = (Ray) {orig, dirNorm};
        HitRec shadowRec;
        if (!hitScene(scene, shadowRay, 0.001f, dist, &shadowRec))
            visibility += 1.0f;
    }

    visibility /= samples;
    if (visibility <= 0.0f)
        return (V3) {0, 0, 0};

    V3 e = v3InvScale(light->base.intensity, lightDirMag * lightDirMag);
    float NdotL = fmaxf(0.0f, v3Dot(rec->normal, lightDirNorm));
    V3 lighting = v3Scale(e, NdotL);
    return v3Scale(lighting, visibility);
}


V3 ambientIlluminate(Light* self, Scene* scene, HitRec* rec, RNG* rng) {
    return self->intensity;
}
