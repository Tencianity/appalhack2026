#ifndef WINDOW_H
#define WINDOW_H

#include "scene/scene.h"

#include "util/ui.h"
#include "util/thread.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect outerRect;
    SDL_Rect innerRect;
    SDL_Rect textRect;
} HudBox;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect rect;
    Scene* scene;
} SceneBox;

typedef struct {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Rect rect;
    Slider* sliders[256];
    int sldrCount;
} UIBox;

int checkInit();

TTF_Font* createFont();

SDL_Window* createWindow(int width, int height);

SDL_Renderer* createRenderer(SDL_Window* window);

SDL_Texture* createSceneTexture(SDL_Window* window, 
        SDL_Renderer* renderer, int width, int height);

HudBox initHudBox(SDL_Window* window, SDL_Renderer* renderer, 
        int wWidth, int wHeight);

SceneBox initSceneBox(SDL_Window* window, SDL_Renderer* renderer, 
        int wWidth, int wHeight);

UIBox initUIBox(SDL_Window* window, SDL_Renderer* renderer, 
        int wWidth, int wHeight);

void drawHudBox(HudBox* box);

void drawSceneBox(SceneBox* box, ThreadPool* pool);

void drawUIBox(UIBox* box, V3 mousePos, int mouseDown);

void updateFpsText(HudBox* box, TTF_Font* font, float fps);

void updateObjs(Scene* scene, UIBox uiBox);

int runWindow(int height, int width);

static inline void freeSliders(UIBox* box) {
    for (int i = 0; i < box->sldrCount; i++) {
        free(box->sliders[i]);
    }
}


#endif
