#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "util/ui.h"
#include "math/vector.h"

static inline int square(int num) { return num * num; }

#define TRUE 1
#define FALSE 0

Slider speedSlider;
Slider camSlider;

Slider createSlider(int x, int y, int width, int height, char* label) {
    Slider s;
    
    // Create the border of the slider
    s.border = (SDL_Rect) {x, y, width, height}; 
    s.value = 0.f;

    // Calculate where the button should be in world coords
    s.btnWorldPos = (V3) {(x + width / 10), (y + height / 2.f), 0.f};

    // Button radius is 1/50 the area
    s.label = label;
    s.btnRadius = 10;
    return s;
}

void drawSlider(SDL_Renderer* renderer, Slider slider) {

    // Draw the border
    // SDL_Color grayish = {50, 50, 50, 255};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 150);
    SDL_RenderDrawRect(renderer, &(slider.border));
    SDL_RenderFillRect(renderer, &(slider.border));
    
    // SDL_Color aqua = {0, 50, 255, 255};
    SDL_SetRenderDrawColor(renderer, 0, 50, 255, 255);
    
    // Draw the draggable button
    for (int col = 0; col < slider.border.w; col++) {
        for (int row = 0; row < slider.border.h; row++) {
            
            int leftBorder = slider.border.x;
            int rightBorder = slider.border.x + slider.border.w;

            // Represents how far the current point is fron the center of the button
            V3 v = {slider.border.x + col, slider.border.y + row, 0.f};
            v = v3Sub(slider.btnWorldPos, v);
            float dist = sqrt( square(v.x) + square(v.y) );
            
            // Draw button within a radius (circle)
            if (dist < slider.btnRadius) {
                SDL_SetRenderDrawColor(renderer, 0, 50, 255, 255);
                SDL_RenderDrawPoint(renderer, slider.border.x + col, slider.border.y + row);
            }
            // Draw line that the button drags across
            else if (slider.border.x + col > leftBorder + 10
                    && slider.border.x + col < rightBorder - 10
                    && row == slider.border.h / 2) {
                SDL_SetRenderDrawColor(renderer, 200, 100, 0, 255);
                SDL_RenderDrawPoint(renderer, slider.border.x + col, slider.border.y + row);
            }
            
        }
    }
}

void updateSlider(SDL_Renderer* renderer, Slider* slider, V3 mousePos, int mouseDown) {

    // Make the draggable button follow the mouse when dragged 
    // and update slider value.
    if (mouseDown && abs((int) (slider->btnWorldPos.x - mousePos.x)) < 2*slider->btnRadius
            && abs((int) (slider->btnWorldPos.y - mousePos.y)) < 2*slider->btnRadius) {
        slider->value = mousePos.x - (slider->border.x + slider->border.w / 10);
        slider->btnWorldPos = (V3) {
            (slider->border.x + slider->border.w / 10) + slider->value,
            (slider->border.y + slider->border.h / 2), 0.f};
    }
    drawSlider(renderer, *slider);
}

void initUI() {
    // Create the slider representing speed multiplier
    speedSlider = createSlider(10, 100, 100, 50, "Speed");
    speedSlider.value = 5.f;

    camSlider = createSlider(140, 100, 100, 50, "Camera");
    camSlider.value = 0.f;
}

void drawUI(SDL_Renderer* renderer, int mouseDown, V3 mousePos) {

    updateSlider(renderer, &speedSlider, mousePos, mouseDown);
    updateSlider(renderer, &camSlider, mousePos, mouseDown);
    SDL_RenderPresent(renderer);
}


