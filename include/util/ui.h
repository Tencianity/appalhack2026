#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>
#include <math/vector.h>

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

Slider createSlider(int x, int y, int width, int height, char* label);

void drawSlider(SDL_Renderer* renderer, Slider slider, TTF_Font* font);

void updateSlider(SDL_Renderer* renderer, Slider* slider, V3 mousePos, int mouseDown, TTF_Font* font);

void initUI();

void drawUI(SDL_Renderer* renderer, int mouseDown, V3 mousePos);

#endif
