#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "util/ui.h"
#include "math/vector.h"
#include "util/window.h"

#define TRUE 1
#define FALSE 0

TTF_Font* font;

Slider* createSlider(int x, int y, int width, int height, char* label) {
    Slider* s = malloc(sizeof(Slider));
    
    // Create the border of the slider
    s->border = (SDL_Rect) {x, y, width, height}; 
    s->value = 0.f;

    // Calculate where the button should be in world coords
    s->btnWorldPos = (V3) {(x + width / 10), (y + height / 2.f), 0.f};

    // Button radius is 1/50 the area
    s->label = label;
    s->btnRadius = 10;
    return s;
    printf("Slider <%s> created!\n", label);
}

int updateSlider(SDL_Renderer* renderer, Slider* slider, V3 mousePos, int mouseDown, TTF_Font* font) {

    // Make the draggable button follow the mouse when dragged 
    // and update slider value.
    if (mouseDown && abs((int) (slider->btnWorldPos.x - mousePos.x)) < 2*slider->btnRadius
            && abs((int) (slider->btnWorldPos.y - mousePos.y)) < 2*slider->btnRadius) {
        slider->value = mousePos.x - (slider->border.x + slider->border.w / 10);
        if (slider->value < 0) slider->value = 0;
        if (slider->value > 75) slider->value = 75;
        slider->btnWorldPos = (V3) {
            (slider->border.x + slider->border.w / 10) + slider->value,
            (slider->border.y + slider->border.h / 2), 0.f
        };
        return 1;
    }
    return 0;
}
