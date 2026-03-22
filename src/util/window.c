#include "util/window.h"
#include "util/ui.h"

#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/tracer.h"
#include "scene/light.h"
#include "scene/color.h"

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


int checkInit() {
    int hasError = 0;
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("ERROR, couldn't initialize SDL_VIDEO: %s\n", SDL_GetError());
        hasError = 1;
    }
    if (TTF_Init() != 0) {
        printf("ERROR, couldn't initialize TTF: %s\n", TTF_GetError());
        SDL_Quit();
        hasError = 1;
    }

    return hasError;
}


TTF_Font* createFont() {
    TTF_Font* font;
    font = TTF_OpenFont(
        "assets/fonts/JetBrainsMono-Regular.ttf",
        24
    );

    if (!font) {
        printf("ERROR, couldn't load font: %s\n", TTF_GetError());
        SDL_Quit();
    }

    return font;
}


SDL_Window* createWindow(int width, int height) {
    SDL_Window* window;
    window = SDL_CreateWindow(
        "BoxBoi", 
        SDL_WINDOWPOS_CENTERED, 
        SDL_WINDOWPOS_CENTERED, 
        width, 
        height, 
        0
    );

    if (!window) {
        printf("ERROR, couldn't initialize window: %s\n", SDL_GetError());
        SDL_Quit();
    }
    
    return window;
}


SDL_Renderer* createRenderer(SDL_Window* window) {
    SDL_Renderer* renderer;
    renderer = SDL_CreateRenderer(
        window, 
        -1, 
        SDL_RENDERER_ACCELERATED
    );

    if (!renderer) {
        printf("ERROR, couldn't initialize renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

    return renderer;
}


SDL_Texture* createSceneTexture(SDL_Window* window, 
        SDL_Renderer* renderer, int width, int height) {
    
    SDL_Texture* texture;
    texture = SDL_CreateTexture(
        renderer, 
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );

    if (!texture) {
        printf("ERROR, couldn't initialize texture: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_DestroyRenderer(renderer);
        SDL_Quit();
    }

    return texture;
}


HudBox initHudBox(SDL_Window* window, SDL_Renderer* renderer, 
        int wWidth, int wHeight) {

    HudBox box;
    box.window = window;
    box.renderer = renderer;
    box.outerRect = (SDL_Rect) {
        0, 
        0, 
        wWidth, 
        wHeight / 10
    };
    box.innerRect = (SDL_Rect) {
        box.outerRect.x + 2, 
        box.outerRect.y + 2,
        box.outerRect.w - 4,
        box.outerRect.h - 4
    };

    return box;
}

SceneBox initSceneBox(SDL_Window* window, SDL_Renderer* renderer, 
        int wWidth, int wHeight) {

    int sWidth = wWidth;
    int sHeight = wHeight * 9 / 10;

    Scene* scene = (Scene*)calloc(1, sizeof(Scene));
    scene->width = sWidth;
    scene->height = sHeight;
    scene->buffer = malloc(sizeof(uint32_t) * sWidth * sHeight);
    scene->cam = createCamera((float) sWidth / sHeight);
    scene->objCount = 0;
    scene->lightCount = 0;

    SDL_Rect sceneRect = {
        0, 
        wHeight / 10,
        sWidth, 
        sHeight
    };

    SceneBox box;
    box.window = window;
    box.renderer = renderer;
    box.texture = createSceneTexture(
        window, 
        renderer,
        sWidth,
        sHeight
    );
    box.rect = sceneRect;
    box.scene = scene;

    // Here temporarally
    PointLight* light = createPointLight(
        (V3) {0.8f, 0.8f, 0.8f},
        (V3) {2.0f, 2.0f, 2.0f}
    );
    box.scene->lights[scene->lightCount++] = (Light*) light;

    // Here temporarally
    V3 sphereColor = {0.2f, 0.2f, 0.8f};
    Mat sphereMat = (Mat) {sphereColor, 0, 0, 0, 0};
    Sphere* sphere = createSphere(
        (V3) {0, 0, -1}, // origin
        0.25f, // radius
        sphereMat // material
    );
    box.scene->objects[scene->objCount++] = (Surface*) sphere;

    return box;
}


void drawHudBox(HudBox* box) {
    SDL_SetRenderDrawColor(box->renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(box->renderer, &box->innerRect);
    SDL_SetRenderDrawColor(box->renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(box->renderer, &box->innerRect);

    if (box->texture)
        SDL_RenderCopy(box->renderer, box->texture, NULL, &box->textRect);
}


void drawSceneBox(SceneBox* box) {
    renderScene(box->scene);
    SDL_UpdateTexture(box->texture, NULL, box->scene->buffer, 
            box->scene->width * sizeof(uint32_t));
    SDL_RenderCopy(box->renderer, box->texture, NULL, &box->rect);
    SDL_RenderPresent(box->renderer);
}


void updateFpsText(HudBox* box, TTF_Font* font, float fps) {
    if (box->texture) SDL_DestroyTexture(box->texture);
            
    char fpsText[32];
    sprintf(fpsText, "FPS: %.1f", fps);
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface* surface = TTF_RenderText_Solid(font, fpsText, white);
    box->texture = SDL_CreateTextureFromSurface(box->renderer, surface);
    box->textRect = (SDL_Rect) {
        10, 
        box->innerRect.y + (box->innerRect.h - surface->h) / 2,
        surface->w, 
        surface->h
    };
    SDL_FreeSurface(surface);
}


int runWindow(int width, int height) {
    int hasInitError = checkInit();
    if (hasInitError) return 1;
    TTF_Font* font = createFont();
    if (!font) return 1;
    SDL_Window* window = createWindow(width, height);
    if (!window) return 1;
    SDL_Renderer* renderer = createRenderer(window);
    if (!renderer) return 1;
    SDL_Texture* texture = createSceneTexture(window, renderer, width, height);
    if (!texture) return 1;

    SDL_Event event;
    
    HudBox hudBox = initHudBox(window, renderer, width, height);
    SceneBox sceneBox = initSceneBox(window, renderer, width, height);
    initUI();
   
    Uint32 lastTime = SDL_GetTicks();
    int running = 1;
    int frameCount = 0;
    float fps = 0;
    int mouseDown = 0;
    V3 mousePos;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            if (event.type == SDL_MOUSEBUTTONDOWN) mouseDown = 1;
            if (event.type == SDL_MOUSEBUTTONUP) mouseDown = 0;
            if (event.type == SDL_MOUSEMOTION) {
                mousePos = (V3) {event.motion.x, event.motion.y, 0.f};
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        frameCount++;
        Uint32 currentTime = SDL_GetTicks();

        if (currentTime - lastTime >= 1000) {
            fps = frameCount;
            frameCount = 0;
            lastTime = currentTime;
            updateFpsText(&hudBox, font, fps);
        }

        drawHudBox(&hudBox);
        drawSceneBox(&sceneBox);
        drawUI(renderer, mouseDown, mousePos);
    }

    free(sceneBox.scene->buffer);
    SDL_DestroyTexture(hudBox.texture);
    SDL_DestroyTexture(sceneBox.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
