#include "scene/tracer.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/ray.h"
#include "scene/hit.h"
#include "scene/color.h"

#include "math/random.h"

#include <stdint.h>
#include <stdlib.h>


void renderScene(Scene* scene, int frameSeed) {
    for (int i = 0; i < scene->height; i++) {
        for (int j = 0; j < scene->width; j++) {
            float u = (float)j / (scene->width - 1);
            float v = (float)i / (scene->height - 1);
           
            Ray ray = castRay(scene->cam, u, v);
            uint32_t seed = (j * 1973) + (i * 9277) + frameSeed;
            RNG rng;
            initRNG(&rng, seed);
            RGBA rgba = colorRay(ray, scene, &rng);

            uint32_t A = rgba.a << 24;
            uint32_t R = rgba.r << 16;
            uint32_t G = rgba.g << 8;
            uint32_t B = rgba.b;
            
            uint32_t color = A | R | G | B;
            scene->buffer[i * scene->width + j] = color;
        }
    }
}


RGBA colorRay(Ray ray, Scene* scene, RNG* rng) {
    HitRec rec;
    if (hitScene(scene, ray, 0.001f, 1000.0f, &rec)) {
        V3 lightColor = {0, 0, 0};
        for (int i = 0; i < scene->lightCount; i++) {
            Light* light = scene->lights[i];
            V3 contribution = light->illuminate(light, scene, &rec, rng);
            lightColor = v3Add(lightColor, contribution);
        }

        V3 color = v3Mult(rec.mat.color, lightColor);
        color = v3Scale(v3Clamp(color, 0.0f, 1.0f), 255.0f);
        return (RGBA) {color.x, color.y, color.z, 255};
    }
    return (RGBA) {0, 0, 0, 255}; // Should be black if not hit
}


int hitScene(Scene* scene, Ray ray, float tMin, 
        float tMax, HitRec* rec) {
   
    HitRec temp;
    int hasHit = 0; // Boolean false
    float closest = tMax;

    for (int i = 0; i < scene->objCount; i++) {
        Surface* obj = scene->objects[i];
        if (obj->hit(obj, ray, tMin, closest, &temp)) {
            hasHit = 1; // Boolean true
            closest = temp.t;
            *rec = temp;
        }
    }
    return hasHit;
}
