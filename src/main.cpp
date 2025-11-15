#include "Graphics.h"
#include "Config.h"
#include <iostream>
#include <SDL_image.h>
#include <vector>
#include <memory>   // pour std::unique_ptr
#include <SDL_mixer.h>
#include <ctime>
#include <array>

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

    std::srand(std::time(nullptr)); // init random

    Graphics graphics;

    Menu menu(graphics.getRenderer());
    menu.configureParameters();

    int nbGuerriers = menu.getNbGuerriers();
    std::string typeGuerrier = menu.getTypeGuerriers();

    std::cout << "---- Parametres choisis ----\n";
    std::cout << "Nb Guerriers : " << nbGuerriers << "\n";

    std::vector<Entity*> entities;
    std::array<std::string, 4> types = {"Guerrier", "Archer", "Mage", "Golem"};

    for (int i = 0; i < nbGuerriers; i++) {
        std::string entity_type = "";
        int width, height;
        SDL_GetWindowSize(graphics.getWindow(), &width, &height);
        int x = std::rand() % (width-50) + 50; // entre 50 et width-50
        int y = std::rand() % (height-50) + 50; // entre 50 et height-50
        int index_type = std::rand() % (types.size()-1);
        index_type = 3;
        switch (index_type) {
            case 0: // Guerrier
                entities.push_back(new Guerrier(x, y, graphics.getRenderer()));
                break;
            case 1: // Archer
                entities.push_back(new Archer(x, y, graphics.getRenderer()));
                break;
            case 2: // Mage
                entities.push_back(new Mage(x, y, graphics.getRenderer()));
                break;
            case 3: // Golem
                entities.push_back(new Golem(x, y, graphics.getRenderer()));
                break;
            default:
                entities.push_back(new Entity(x, y, graphics.getRenderer()));
                break;
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