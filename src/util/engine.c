#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include "scene/surface.h"
#include "scene/scene.h"
#include "util/engine.h"
#include "math/vector.h"

/**
 * May god have mercy on any soul whom wishes to parse this code.
 */

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
            // printf("Could be anything... be on your toes.\n");
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
        variance *= sign / 1000.f;

        // Find bounding box that contains spheres
        Box* bounds;
        for (int i = 0; i < scene->objCount; i++) {
            if (scene->objects[i]->type == BOX_TYPE) {
                bounds = (Box*) scene->objects[i];
            }
        }
        
        if (bounds != NULL) {
            // Front, back, left, right, top, bottom
            // Assign each quad as a side of a cube
            myCube cube = {
                *bounds->faces[0],
                *bounds->faces[1],
                *bounds->faces[2],
                *bounds->faces[3],
                *bounds->faces[4],
                *bounds->faces[5]
            };
            V3 boxMid = v3Sub(bounds->faces[0]->origin, bounds->faces[1]->origin);
            
            // Update collision surface normal based on scene boundaries
            if (nextLoc.x - sphere->radius <= cube.faceLeft.origin.x) {
                wallNorm = v3Add(wallNorm, (V3) {1.f, 0.f, 0.f});
                doesCollision = TRUE;
            }
            else if (nextLoc.x + sphere->radius >= cube.faceRight.origin.x) {
                wallNorm = v3Add(wallNorm, (V3) {-1.f, 0.f, 0.f});
                doesCollision = TRUE;
            }
            else if (nextLoc.y - sphere->radius <= cube.faceTop.origin.y) {
                wallNorm = v3Add(wallNorm, (V3) {0.f, 1.f, 0.f});
                doesCollision = TRUE;
            }
            else if (nextLoc.y + sphere->radius >= cube.faceBottom.origin.y) {
                wallNorm = v3Add(wallNorm, (V3) {0.f, -1.f, 0.f});
                doesCollision = TRUE;
            }
            else if (nextLoc.z - sphere->radius <= cube.faceFront.origin.z) {
                wallNorm = v3Add(wallNorm, (V3) {0.f, 0.f, 1.f});
                doesCollision = TRUE;
            }
            else if (nextLoc.z + sphere->radius >= cube.faceBack.origin.z) {
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
        }           
        // Update surface velocity and increment location
        vel = v3Normalize(vel); // Normalize velocity
        sphere->base.velocity = vel;
        vel = v3Add(vel, (V3) {variance, variance, variance});
        vel = v3Scale(vel, speed); // Scale velocity by speed
        sphere->center = v3Add(loc, vel);
}