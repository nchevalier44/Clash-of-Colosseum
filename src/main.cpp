#include "Graphics.h"
#include "Config.h"
#include <iostream>

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Erreur SDL_Init: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (TTF_Init() != 0) {
        std::cerr << "Erreur TTF_Init: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    Graphics graphics;

    Menu menu(graphics.getRenderer());
    menu.configureParameters();

    int nbGuerriers = menu.getNbGuerriers();
    int pvBase = menu.getPvBase();

    std::cout << "Nb Guerriers: " << nbGuerriers << std::endl;
    std::cout << "PV Base: " << pvBase << std::endl;

    bool running = true;
    while (running) {
        graphics.update(&running);
        SDL_Delay(16);
    }

    TTF_Quit();
    SDL_Quit();
    return 0;
}

