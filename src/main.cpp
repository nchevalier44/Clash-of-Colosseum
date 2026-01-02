#include "Graphics.h"
#include "Config.h"
#include "GameMenu.h"
#include "Weapons/Projectile.h"
#include "Entities/Guerrier.h"
#include "Entities/Archer.h"
#include "Entities/Mage.h"
#include "Entities/Golem.h"
#include <iostream>
#include <SDL_image.h>
#include <vector>
#include <memory>   // pour std::unique_ptr
#include <SDL_mixer.h>
#include <ctime>
#include <array>

bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) std::cerr << Mix_GetError() << std::endl;
    if (TTF_Init() != 0) return false;
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) return false;

    std::srand(std::time(nullptr));
    return true;
}

void start_game(bool* keep_playing) {
    Graphics graphics;
    Menu menu(graphics.getRenderer());

    menu.configureParameters();

    Projectile::globalSpeedMultiplier = menu.getProjectileSpeedMultiplier();
    graphics.setMutationTypeRate(menu.getMutationTypeRate());
    graphics.setMutationStatsRate(menu.getMutationStatsRate());
    graphics.setShowHealthBars(menu.getShowHealthBars());

    // Transmission de la nouvelle option
    graphics.setSameTypePeace(menu.getSameTypePeace());

    int nbGuerriers = menu.getNbGuerriers();
    std::cout << "Paix meme type : " << (menu.getSameTypePeace() ? "ON" : "OFF") << "\n";

    // --- NOUVEAU SPAWN ALEATOIRE ---
    std::vector<Entity*> entities;
    std::array<std::string, 4> types = {"Guerrier", "Archer", "Mage", "Golem"};

    int winW, winH;
    SDL_GetWindowSize(graphics.getWindow(), &winW, &winH);

    for (int i = 0; i < nbGuerriers; i++) {
        // Spawn aléatoire sur toute la map (avec une petite marge de 30px pour pas être dans le mur)
        int x = std::rand() % (winW - 60) + 30;
        int y = std::rand() % (winH - 60) + 30;

        int index_type = std::rand() % (types.size());

        switch (index_type) {
            case 0: entities.push_back(new Guerrier(x, y, graphics.getRenderer())); break;
            case 1: entities.push_back(new Archer(x, y, graphics.getRenderer())); break;
            case 2: entities.push_back(new Mage(x, y, graphics.getRenderer())); break;
            case 3: entities.push_back(new Golem(x, y, graphics.getRenderer())); break;
            default: entities.push_back(new Entity(x, y, graphics.getRenderer())); break;
        }
    }

    graphics.setEntities(entities);

    Mix_HaltMusic();
    Mix_PlayMusic(graphics.getGameMusic(), -1);

    graphics.startAllEntitiesThread();
    bool running = true;
    while (running) {
        graphics.update(&running, keep_playing);
        SDL_Delay(16);
    }
}

int main() {
    if (!init()) return 1;

    bool keep_playing = true;
    while (keep_playing) {
        start_game(&keep_playing);
    }

    IMG_Quit(); TTF_Quit(); Mix_CloseAudio(); Mix_Quit(); SDL_Quit();
    return 0;
}