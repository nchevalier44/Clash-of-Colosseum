#include "Graphics.h"
#include "Config.h"
#include <iostream>
#include <SDL_image.h>
#include <vector>
#include <memory>   // pour std::unique_ptr
#include <SDL_mixer.h>

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "Erreur SDL_Init: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::cerr << "Erreur Mix_OpenAudio: " << Mix_GetError() << std::endl;
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
    std::string typeGuerrier = menu.getTypeGuerriers();

    std::cout << "---- Parametres choisis ----\n";
    std::cout << "Nb Guerriers : " << nbGuerriers << "\n";
    std::cout << "Type Guerrier: " << typeGuerrier << "\n";

    std::vector<Entity*> entities;
    for (int i = 0; i < nbGuerriers; i++) {
        int x = 100 + i * 100;
        int y = 300;
        if (typeGuerrier == "Guerrier") {
            entities.push_back(new Guerrier(x, y, graphics.getRenderer()));
        }
        else if (typeGuerrier == "Archer") {
            entities.push_back(new Archer(x, y, graphics.getRenderer()));
        }
        else if (typeGuerrier == "Mage") {
            entities.push_back(new Mage(x, y, graphics.getRenderer()));
        }
        else if (typeGuerrier == "Tank") {
            entities.push_back(new Tank(x, y, graphics.getRenderer()));
        }
        else {
            entities.push_back(new Entity(x, y, graphics.getRenderer()));
        }

    }
    graphics.setEntities(entities);
    // ----- Boucle principale -----
    // Stop musique du menu (au cas où elle tourne encore)
    Mix_HaltMusic();

    // Lance la musique du jeu
    Mix_PlayMusic(graphics.getGameMusic(), -1);

    bool running = true;
    while (running) {
        graphics.update(&running);
        SDL_Delay(16); // ~60 FPS
    }
    IMG_Quit();
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
    return 0;
}