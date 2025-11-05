#include "Graphics.h"
#include "Config.h"
#include <iostream>
#include <SDL_image.h>
#include <vector>
#include <memory>   // pour std::unique_ptr

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

    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) {
        std::cerr << "Erreur IMG_Init: " << IMG_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    Graphics graphics;

    Menu menu(graphics.getRenderer());
    menu.configureParameters();

    int nbGuerriers = menu.getNbGuerriers();
    int pvBase = menu.getPvBase();
    int dureeCombat = menu.getDureeCombat();
    std::string typeGuerrier = menu.getTypeGuerriers();

    std::cout << "---- Parametres choisis ----\n";
    std::cout << "Nb Guerriers : " << nbGuerriers << "\n";
    std::cout << "PV Guerriers : " << pvBase << "\n";
    std::cout << "Duree combat : " << dureeCombat << "s\n";
    std::cout << "Type Guerrier: " << typeGuerrier << "\n";

    std::vector<std::unique_ptr<Entity>> entities;
    for (int i = 0; i < nbGuerriers; i++) {
        int x = 100 + i * 100;
        int y = 300;
        if (typeGuerrier == "Guerrier") {
            entities.push_back(std::make_unique<Guerrier>(x, y, 20, pvBase));
        }
        else if (typeGuerrier == "Archer") {
            entities.push_back(std::make_unique<Archer>(x, y, 20, pvBase));
        }
        else if (typeGuerrier == "Mage") {
            entities.push_back(std::make_unique<Mage>(x, y, 20, pvBase));
        }
        else if (typeGuerrier == "Tank") {
            entities.push_back(std::make_unique<Tank>(x, y, 30, pvBase * 2));
        }
        else {
            entities.push_back(std::make_unique<Entity>(x, y, 20, pvBase));
        }
    }
    graphics.setEntities(entities);
    // ----- Boucle principale -----
    bool running = true;
    Uint32 startTime = SDL_GetTicks();

    while (running) {
        Uint32 elapsed = SDL_GetTicks() - startTime;
        if (elapsed >= static_cast<Uint32>(dureeCombat) * 1000) {
            std::cout << "\n⏳ Le combat est termine !\n";
            running = false;
        }

        graphics.update(&running);
        SDL_Delay(16); // ~60 FPS
    }

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}
