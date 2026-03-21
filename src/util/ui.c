#include <SDL2/SDL2.h>
#include <SDL2/SDL2_ttf.h>
#include "ui.h"
#include "vector.h"

#define TRUE 1
#define FALSE 0
static int mouseDown;

Slider createSlider(int x, int y, int width, int height) {
    Slider s;
    
    // Create the border of the slider
    s.border = (SDL_Rect) {x, y, width, height}
    s.value = 0.f;
    s.btnPos = 0;

    // Calculate where the button should be in world coords
    s.btnWorldPos = {(s.border.x + width / 100), s.border.y + (height / 2, 0.f)};

    // Button radius is 1/50 the area
    s.btnRadius = height * width / 50;
    return s;
}

void drawSlider(SDL_Renderer* renderer, Slider slider) {
    SDL_Color blackish = {0, 0, 0, 150};
    SDL_SetRenderDrawColor(renderer, blackish);
    SDL_RenderDrawRect(renderer, slider.border);
    
    SDL_Color aqua = {0, 50, 255, 255};
    SDL_SetRenderDrawColor(renderer, aqua);
    int r = slider.btnRadius;
    
    // Draw the draggable button
    for (int col = 0; col < height; col++) {
        for (int row = 0; row < height; row++) {

            int leftBorder = slider.border.x;
            int rightBorder = slider.border.x + slider.border.w;

            // Draw button within a radius (circle)
            if (col*col + row*row < slider.btnWorldPos + r*r) {
                SDL_RenderDrawPoint(renderer, col, row);
            }
            // Draw line button drags across
            else if (col > leftBorder + (leftBorder / 20) && col < rightBorder - (rightBorder / 20)) {
                SDL_RenderDrawPoint(renderer, col, row);
            }
            
        }
    }
}

void updateSlider(Slider slider, V3 mousePos) {
    int mxsqrd = mousePos.x * mousePos.x;
    int mysqrd = mousePos.y * mousePos.y;

    // Make the draggable button follow the mouse when dragged 
    // and update slider value.
    if (mouseDown && mxsqrd + mysqrd < btnWorldRadius) {
        slider.btnPos = mousePos.x;
    }
    slider.value = slider.btnPos;
}

void drawUI(SDL_Renderer* renderer) {
    SDL_Event event;
    V3 mousePos;
    
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            mouseDown = TRUE;
        }
        if (event.type == SDL_MOUSEBUTTONUP) {
            mouseDown = FALSE;
        }
        if (event.type == SDL_MOUSEMOTION) {
            mousePos = (V3) {event.motion.x, event.motion.y, 0.f}
        }
    }

    // Create the slider representing speed multiplier
    Slider speedSlider = createSlider(20, 10, 200, 50);
    speedSlider.value = 5.f;
    int btnWorldRadius = speedSlider.btnWorldPos + speedSlider.btnRadius*speedSlider.btnRadius;

    Slider camSlider = createSlider(20, 10);

    updateSlider(speedSlider, mousePos);
    updateSlider(camSlider, mousePos);
}


