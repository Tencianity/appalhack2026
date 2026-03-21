#ifndef WINDOW_H
#define WINDOW_H

#include "scene/scene.h"

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

void drawHudBox(HudBox* box);

void drawSceneBox(SceneBox* box);

void updateFpsText(HudBox* box, TTF_Font* font, float fps);

int runWindow(int height, int width);


#endif
