#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <math/vector.h>

static inline int square(int num) { return num * num; };

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
    // Text identifying the slider
    char* label;
} Slider;

Slider* createSlider(int x, int y, int width, int height, char* label);

int updateSlider(SDL_Renderer* renderer, Slider* slider, V3 mousePos, int mouseDown, TTF_Font* font);

#endif
