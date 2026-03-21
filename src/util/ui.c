#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "util/ui.h"
#include "math/vector.h"

static inline int square(int num) { return num * num; }

#define TRUE 1
#define FALSE 0

Slider createSlider(int x, int y, int width, int height) {
    Slider s;
    
    // Create the border of the slider
    s.border = (SDL_Rect) {x, y, width, height};
    s.value = 0.f;
    s.btnPos = 0;

    // Calculate where the button should be in world coords
    s.btnWorldPos = (V3) {(x + width / 10), (y + height / 2.f), 0.f};

    // Button radius is 1/50 the area
    s.btnRadius = 10;
    return s;
}

void drawSlider(SDL_Renderer* renderer, Slider slider) {

    // Draw the border
    SDL_Color grayish = {50, 50, 50, 255};
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 150);
    SDL_RenderDrawRect(renderer, &(slider.border));
    SDL_RenderFillRect(renderer, &(slider.border));
    
    SDL_Color aqua = {0, 50, 255, 255};
    SDL_SetRenderDrawColor(renderer, 0, 50, 255, 255);
    
    // Draw the draggable button
    for (int col = 0; col < slider.border.w; col++) {
        for (int row = 0; row < slider.border.h; row++) {
            
            int leftBorder = slider.border.x;
            int rightBorder = slider.border.x + slider.border.w;
            int r = square(slider.btnRadius);
            
            
            // Draw button within a radius (circle)
            if (slider.border.x + col < slider.btnWorldPos.x - slider.border.x + slider.btnRadius
                    && slider.border.y + row < slider.btnWorldPos.y - slider.border.y + slider.btnRadius) {
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
    if (mouseDown && abs(slider->btnWorldPos.x - mousePos.x) < slider->btnRadius
            && abs(slider->btnWorldPos.y - mousePos.y) < slider->btnRadius) {
        slider->btnPos = mousePos.x;
    }
    slider->value = slider->btnPos;
    slider->btnWorldPos = (V3) {
        (slider->border.x + slider->border.w / 10) + slider->btnPos,
        (slider->border.y + slider->border.h / 2) + slider->btnPos, 0.f};
    drawSlider(renderer, *slider);
}

void drawUI(SDL_Renderer* renderer, int mouseDown, V3 mousePos) {

    // Create the slider representing speed multiplier
    Slider speedSlider = createSlider(10, 100, 100, 50);
    speedSlider.value = 5.f;
    
    Slider camSlider = createSlider(140, 100, 100, 50);
    camSlider.value = 0.f;

    updateSlider(renderer, &speedSlider, mousePos, mouseDown);
    updateSlider(renderer, &camSlider, mousePos, mouseDown);
    SDL_RenderPresent(renderer);
}


