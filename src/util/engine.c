#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>

// Delete these includes before make
#include "scene/surface.h"
#include "scene/scene.h"
#include "math/vector.h"

#define FALSE 0
#define TRUE 1

/*

void transpose(Scene* scene, Surface* object) {
    
    switch(object->type) {
        case 1:
            // Velocity and location of the sphere
            V3 vel = object->velocity;
            Sphere* sphere = (Sphere*) object;
            V3 loc = sphere->center;
            
            // Bounds of the scene
            int boundY = scene->buffer.length;
            int boundX = scene->buffer[0].length;
            int boundZ = scene->buffer[0][0].length;

            V3 nextLoc = v3Add(loc, vel);
            int doesCollision = FALSE;

            // Create seeded random float for ball bounce variance
            srand((unsigned int) time(NULL));
            float variance = (float) (rand() % 1000) / 1000.f;
    
            // Update collision surface normal based on scene boundaries
            V3 wallNorm = {0.f, 0.f, 0.f};
            if (nextLoc.x < 0) {
                wallNorm = v3Add(wallNorm, (V3) {1.f, 0.f, 0.f});
                doesCollision = TRUE;
            }
            else if (nextLoc.x > boundX) {
                wallNorm = v3Add(wallNorm, (V3) {-1.f, 0.f, 0.f});
                doesCollision = TRUE;
            }
            if (nextLoc.y < 0) {
                wallNorm = v3Add(wallNorm, (V3) {0.f, 1.f, 0.f});
                doesCollision = TRUE;
            }
            else if (nextLoc.y > boundY) {
                wallNorm = v3Add(wallNorm, (V3) {0.f, -1.f, 0.f});
                doesCollision = TRUE;
            }
            if (nextLoc.z < 0) {
                wallNorm = v3Add(wallNorm, (V3) {0.f, 0.f, 1.f});
                doesCollision = TRUE;
            }
            else if (nextLoc.z > boundZ) {
                wallNorm = v3Add(wallNorm, (V3) {0.f, 0.f, -1.f});
                doesCollision = TRUE;
            }

            // Reflect off wall if a collision occurs this frame
            if (doesCollision) {
                vel = v3Reflect(vel, wallNorm);
            }
    
            // Update sphere velocity and increment location
            vel = v3Normalize(vel);
            sphere->velocity = vel;
            sphere->point = v3Add(loc, vel);
            break;
        default:
            break;
    }
}

*/
