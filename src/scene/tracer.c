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
        // To be messed with
        // float light = rec.normal.y * 0.5f + 0.5f;
        return rec.mat.color;
    }
    return (RGBA) {0, 0, 0, 1}; // Should be black if not hit
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
