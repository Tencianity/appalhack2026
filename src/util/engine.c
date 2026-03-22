#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "scene/surface.h"
#include "scene/scene.h"
#include "util/engine.h"
#include "math/vector.h"

#define FALSE 0
#define TRUE 1

/**
 * Updates the location of a surface using its velocity
 * and the bounds of the scene.
 * 
 * @param scene The scene struct
 * @param surface The surface to transpose
 */
void transpose(Scene* scene, Surface* object, float speed) {
    // Not a surface
    switch(object->type) {
        case (SPHERE_TYPE):
            mvSphere(scene, (Sphere*) object, speed);
            break;
        default:
            printf("Could be anything... be on your toes.\n");
    }
}

void mvSphere(Scene* scene, Sphere* sphere, float speed) {
        // Velocity and location of the surface
        V3 vel = sphere->base.velocity;
        vel = v3Normalize(vel);
        vel = v3Scale(vel, speed);
        V3 loc = sphere->center;

        V3 nextLoc = v3Add(loc, vel);
        V3 wallNorm = {0.f, 0.f, 0.f};
        int doesCollision = FALSE;

        // Create seeded random float for ball bounce 0.f
        srand((unsigned int) time(NULL));
        float variance = (float) (rand() % 1000);
        float sign = (float) ((int) variance % 1); // -1.f or 1.f
        variance *= sign / 10000.f;
        
        // Update collision surface normal based on scene boundaries
        if (nextLoc.x - sphere->radius < -1) {
            wallNorm = v3Add(wallNorm, (V3) {1.f, 0.f, 0.f});
            doesCollision = TRUE;
        }
        else if (nextLoc.x + sphere->radius > 1) {
            wallNorm = v3Add(wallNorm, (V3) {-1.f, 0.f, 0.f});
            doesCollision = TRUE;
        }
        else if (nextLoc.y - sphere->radius < -1) {
            wallNorm = v3Add(wallNorm, (V3) {0.f, 1.f, 0.f});
            doesCollision = TRUE;
        }
        else if (nextLoc.y + sphere->radius > 1) {
            wallNorm = v3Add(wallNorm, (V3) {0.f, -1.f, 0.f});
            doesCollision = TRUE;
        }
        else if (nextLoc.z - sphere->radius < -2) {
            wallNorm = v3Add(wallNorm, (V3) {0.f, 0.f, 1.f});
            doesCollision = TRUE;
        }
        else if (nextLoc.z + sphere->radius > -1) {
            wallNorm = v3Add(wallNorm, (V3) {0.f, 0.f, -1.f});
            doesCollision = TRUE;
        }
        else {
            doesCollision = FALSE;
        }
        // Reflect off wall if a collision occurs this frame
        if (doesCollision == TRUE) {
            vel = v3Reflect(vel, wallNorm);
        }
        
        // Update surface velocity and increment location
        vel = v3Normalize(vel); // Normalize velocity
        sphere->base.velocity = vel;
        vel = v3Add(vel, (V3) {variance, variance, variance});
        vel = v3Scale(vel, speed); // Scale velocity by speed
        sphere->center = v3Add(loc, vel);
}