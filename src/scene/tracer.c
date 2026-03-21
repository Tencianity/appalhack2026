#include "scene/tracer.h"
#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/ray.h"
#include "scene/hit.h"
#include "scene/color.h"

#include <stdint.h>
#include <stdlib.h>


void renderScene(Scene* scene) {
    for (int i = 0; i < scene->height; i++) {
        for (int j = 0; j < scene->width; j++) {
            float u = (float)j / (scene->width - 1);
            float v = (float)i / (scene->height - 1);
           
            Ray ray = castRay(scene->cam, u, v);
            RGBA rgba = colorRay(ray, scene);

            uint32_t A = rgba.a << 24;
            uint32_t R = rgba.r << 16;
            uint32_t G = rgba.g << 8;
            uint32_t B = rgba.b;
            
            uint32_t color = A | R | G | B;
            scene->buffer[i * scene->width + j] = color;
        }
    }
}


RGBA colorRay(Ray ray, Scene* scene) {
    HitRec rec;
    if (hitScene(scene, ray, 0.001f, 1000.0f, &rec)) {
        V3 color = rec.mat.color;
        for (int i = 0; i < scene->lightCount; i++) {
            Light* light = scene->lights[i];
            V3 contribution = light->illuminate(light, scene, &rec);
            color = v3Add(color, contribution);
        }
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


int hitShadow(Scene* scene, HitRec* rec, V3 dirNorm, float dirMag) {
    V3 orig = v3Add(rec->point, v3Scale(dirNorm, 0.00001f));
    Ray ray = (Ray){orig, dirNorm};
    HitRec sRec;
    return hitScene(scene, ray, 0.001f, dirMag, &sRec);
}
