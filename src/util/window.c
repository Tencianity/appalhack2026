#include "util/window.h"
#include "util/ui.h"
#include "util/engine.h"
#include "util/thread.h"

#include "scene/scene.h"
#include "scene/camera.h"
#include "scene/tracer.h"
#include "scene/light.h"
#include "scene/color.h"

#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
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

    SceneBox box;
    box.window = window;
    box.renderer = renderer;
    box.texture = createSceneTexture(
        window, 
        renderer,
        sWidth,
        sHeight
    );
    box.rect = (SDL_Rect) {
        0, 
        wHeight / 10,
        sWidth, 
        sHeight
    };
    box.scene = scene;

    initSceneLights(scene);
    initSceneSurfaces(scene);
    return box;
}


UIBox initUIBox(SDL_Window* window, SDL_Renderer* renderer,
        int wWidth, int wHeight) {
            
    UIBox box;
    box.window = window;
    box.renderer = renderer;
    box.rect = (SDL_Rect) {
        (wWidth / 10),
        0,
        wWidth / 10,
        wHeight * 9 / 10
    };
    box.font = TTF_OpenFont("assets/fonts/JetBrainsMono-Regular.ttf", 15);

    // Create the slider representing speed multiplier
    Slider* speedSlider = createSlider(150, 100, 100, 50, "Speed");
    speedSlider->value = 5.f;
    Slider* camSlider = createSlider(150, 170, 100, 50, "Camera");
    camSlider->value = 0.f;
    box.sldrCount = 0;
    box.sliders[box.sldrCount++] = speedSlider;
    box.sliders[box.sldrCount++] = camSlider;
    
    return box;
}


void initSceneLights(Scene* scene) {
    PointLight* pLight = createPointLight(
        (V3) {30.0f, 30.0f, 30.0f},
        (V3) {3.0f, 2.0f, 3.0f}
    );
    addLight(scene, (Light*) pLight);

    AmbientLight* aLight = createAmbientLight(
        (V3) {0.2f, 0.2f, 0.2f}
    );
    addLight(scene, (Light*) aLight);
}


void initSceneSurfaces(Scene* scene) {
    V3 groundColor = (V3) {0.75f, 0.75f, 0.75f};
    Mat groundMat = (Mat) {groundColor, 0, 0, 0, 0};
    Plane* ground = createPlane(0.0f, groundMat);
    addSurface(scene, (Surface*) ground);

    V3 sphereColor = (V3) {0.71f, 0.12f, 0.92f};
    Mat sphereMat = (Mat) {sphereColor, 0, 0, 0, 0};
    Sphere* sphere = createSphere(
        (V3) {0, 1, -1},
        0.25f,
        sphereMat
    );
    sphere->base.velocity = (V3) {1.f, 0.5f, 0.4f};
    addSurface(scene, (Surface*) sphere);

    V3 cubeColor = {0.41f, 0.12f, 0.92f};
    Mat cubeMat = (Mat) {cubeColor, 0, 0, 0, 0};
    Cube* cube = createCube(
        (V3) {-1, -1, -3},
        (V3) {1, 1, -3},
        cubeMat
    );
    addSurface(scene, (Surface*) cube);
}


void drawHudBox(HudBox* box) {
    SDL_SetRenderDrawColor(box->renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(box->renderer, &box->innerRect);
    SDL_SetRenderDrawColor(box->renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(box->renderer, &box->innerRect);

    if (box->texture)
        SDL_RenderCopy(box->renderer, box->texture, NULL, &box->textRect);
}


void drawSceneBox(SceneBox* box, ThreadPool* pool) {
    renderScene(box->scene, pool);
    SDL_UpdateTexture(box->texture, NULL, box->scene->buffer, 
            box->scene->width * sizeof(uint32_t));
    SDL_RenderCopy(box->renderer, box->texture, NULL, &box->rect);
}


void drawUIBox(UIBox* box, V3 mousePos, int mouseDown) {
    for (int curr = 0; curr < box->sldrCount; curr++) {
        Slider* sptr = box->sliders[curr];
        updateSlider(box->renderer, sptr, mousePos, mouseDown, box->font);
        Slider slider = *sptr;

        // Draw the border
        // SDL_Color grayish = {50, 50, 50, 255};
        SDL_SetRenderDrawColor(box->renderer, 50, 50, 50, 150);
        SDL_RenderDrawRect(box->renderer, &(slider.border));
        SDL_RenderFillRect(box->renderer, &(slider.border));
        
        // SDL_Color aqua = {0, 50, 255, 255};
        SDL_SetRenderDrawColor(box->renderer, 0, 50, 255, 255);
        
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
                    SDL_SetRenderDrawColor(box->renderer, 0, 50, 255, 255);
                    SDL_RenderDrawPoint(box->renderer, slider.border.x + col, slider.border.y + row);
                }
                // Draw line that the button drags across
                else if (slider.border.x + col > leftBorder + 10
                        && slider.border.x + col < rightBorder - 10
                        && row == slider.border.h / 2) {
                    SDL_SetRenderDrawColor(box->renderer, 200, 100, 0, 255);
                    SDL_RenderDrawPoint(box->renderer, slider.border.x + col, slider.border.y + row);
                }

            }
        }
        // Draw the label of the slider
        SDL_Color white = {255, 255, 255, 255};
        SDL_Surface* surface = TTF_RenderText_Solid(box->font, slider.label, white);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(box->renderer, surface);
        SDL_Rect textRect = {slider.border.x - 5, slider.border.y - 5, 30, 20};
        SDL_RenderCopy(box->renderer, texture, NULL, &textRect);
        SDL_FreeSurface(surface);
    }

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


void updateObjs(Scene* scene, UIBox uiBox) {
    for (int i = 0; i < scene->objCount; i++) {
        Surface* obj = scene->objects[i];
        transpose(scene, obj, (uiBox.sliders[0]->value * 0.005f));
    }
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
    UIBox uiBox = initUIBox(window, renderer, width, height);
  
    int threadCount = sysconf(_SC_NPROCESSORS_ONLN);
    ThreadPool* pool = threadpoolCreate(threadCount);
    printf("ThreadPool initialized with {%d} threads.\n", threadCount);

    Uint32 lastTime = SDL_GetTicks();
    int running = 1;
    int frameCount = 0;
    float fps = 0;
    int mouseDown = 0;
    V3 mousePos;
    
    printf("Successfully initialized window.\n");

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
        drawSceneBox(&sceneBox, pool);
        drawUIBox(&uiBox, mousePos, mouseDown);
        //updateObjs(sceneBox.scene, uiBox);
        SDL_RenderPresent(renderer);
    }

    free(sceneBox.scene->buffer);
    SDL_DestroyTexture(hudBox.texture);
    SDL_DestroyTexture(sceneBox.texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    freeSurfaces(sceneBox.scene);
    freeLights(sceneBox.scene);
    freeSliders(&uiBox);
    SDL_Quit();
    return 0;
}
