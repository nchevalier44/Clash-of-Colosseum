#include "Graphics.h"
#include "Config.h"
#include <iostream>
#include <SDL_image.h>
#include <vector>
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
    std::string typeGuerrier = menu.getTypeGuerriers(); // toujours "Guerrier" pour l'instant

    // ----- Création dynamique des entités -----
    std::vector<Entity*> guerriers;
    for (int i = 0; i < nbGuerriers; i++) {
        int x = 50 + i * 50;  // écart horizontal entre guerriers
        int y = 200;           // position verticale fixe
        Entity* g = new Entity(x, y, 20, pvBase);
        guerriers.push_back(g);
    }
    // ----- Boucle principale -----
    bool running = true;
    Uint32 startTime = SDL_GetTicks(); // pour gérer la durée du combat
    while (running) {
        // Si la durée du combat est dépassée, arrêter
        Uint32 elapsed = SDL_GetTicks() - startTime;
        if (elapsed >= (Uint32)dureeCombat * 1000) running = false;

        graphics.update(&running);
        SDL_Delay(16); // ~60 FPS
    }
    // ----- Nettoyage -----
    for (Entity* e : guerriers) {
        delete e;
    }
    guerriers.clear();

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}

