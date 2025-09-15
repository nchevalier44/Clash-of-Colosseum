#include "Graphics.h"

Graphics::Graphics(){
    SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Clash of Colosseum");
    circle = new short[3];
    circle[0] = 200;
    circle[1] = 200;
    circle[2] = 50;
}
SDL_Renderer* Graphics::getRenderer() const { return renderer; }
Graphics::~Graphics() {
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        this->renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    SDL_Quit();
}

void Graphics::update(bool* running){
    SDL_Event event;

    SDL_SetRenderDrawColor(renderer, 230, 198, 34, 0);
    SDL_RenderClear(renderer);

    filledCircleRGBA(renderer, circle[0], circle[1], circle[2], 255, 0, 0, 255);

    SDL_RenderPresent(renderer);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }

        if(event.type == SDL_KEYDOWN){
            switch(event.key.keysym.scancode){
                case SDL_SCANCODE_UP:
                    circle[1] -= 5;
                    break;
                case SDL_SCANCODE_DOWN:
                    circle[1] += 5;
                    break;
                case SDL_SCANCODE_RIGHT:
                    circle[0] += 5;
                    break;
                case SDL_SCANCODE_LEFT:
                    circle[0] -= 5;
                    break;
            }
        }

    }
}