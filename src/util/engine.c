#include <stdlib.h>

// Delete these includes before make
#include "surface.c"
#include "engine.h"
#include "vector.h"

#define FALSE 0
#define TRUE 1

/**
 * Updates the location of a surface using its velocity
 * and the bounds of the scene.
 * 
 * @param scene The scene struct
 * @param surface The surface to transpose
 */
void transpose(Scene* scene, Surface* object) {
    // Not a surface
    switch(object->type) {
        case (SPHERE_TYPE):
            // Velocity and location of the surface
            V3 vel = surface->velocity;
            V3 loc = surface->center;
            Sphere* sphere = (*Sphere) object;
            float speed = sphere->speed;

            // Bounds of the scene
            int boundY = scene->width;
            int boundX = scene->height;

            V3 nextLoc = v3Add(loc, vel);
            int doesCollision = FALSE;

            // Create seeded random float for ball bounce variance
            srand((unsigned int) time(NULL));
            float variance = (float) (rand() % 1000) / 1000.f;
            
            // Update collision surface normal based on scene boundaries
            V3 wallNorm = {0.f, 0.f, 0.f};
            if (nextLoc.x - sphere->radius < 0) {
                wallNorm = v3Add(wallNorm, (V3) {1.f, variance, 0.f});
                doesCollision = TRUE;
            }
            else if (nextLoc.x + sphere->radius > boundX) {
                wallNorm = v3Add(wallNorm, (V3) {-1.f, variance, 0.f});
                doesCollision = TRUE;
            }
            if (nextLoc.y - sphere->radius < 0) {
                wallNorm = v3Add(wallNorm, (V3) {variance, 1.f, 0.f});
                doesCollision = TRUE;
            }
            else if (nextLoc.y + sphere->radius > boundY) {
                wallNorm = v3Add(wallNorm, (V3) {variance, -1.f, 0.f});
                doesCollision = TRUE;
            }

            // Reflect off wall if a collision occurs this frame
            if (doesCollision) {
                vel = v3Reflect(vel, wallNorm);
            }
            
            // Update surface velocity and increment location
            vel = v3Normalize(vel); // Normalize velocity
            vel = v3Scale(vel, speed); // Scale velocity by speed
            object->velocity = vel;
            object->center = v3Add(loc, vel);
    }
}
