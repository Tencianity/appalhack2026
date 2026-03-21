#include <SDL2/SDL2.h>
#include <vector.h>

typedef struct {
    SDL_Rect border;
    // Value represented by the button
    float value;
    // Position of the button based on the value
    int btnPos;
    // Position of the button in world coordinates
    V3 btnWorldPos;
    // Radius of the button
    int btnRadius;
} Slider;