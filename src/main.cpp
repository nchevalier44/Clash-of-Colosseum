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
#include <memory>
#include <SDL_mixer.h>
#include <ctime>
#include <array>
#include <map>


bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) std::cerr << Mix_GetError() << std::endl;
    if (TTF_Init() != 0) return false;
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) return false;

    std::srand(std::time(nullptr));
    return true;
}

void start_game(bool* keep_playing, std::map<std::string, std::string>* parameters) {
    Graphics graphics;

    int winW = std::stoi((*parameters)["window_width"]);
    int winH = std::stoi((*parameters)["window_height"]);

    if (winW != 0 && winH != 0) {
        SDL_SetWindowSize(graphics.getWindow(), winW, winH);
        SDL_SetWindowPosition(graphics.getWindow(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
    }
    Menu menu(graphics.getRenderer(), parameters);

    menu.configureParameters();

    Projectile::globalSpeedMultiplier = menu.getProjectileSpeedMultiplier();
    graphics.setMutationTypeRate(menu.getMutationTypeRate());
    graphics.setMutationStatsRate(menu.getMutationStatsRate());
    graphics.setShowHealthBars(menu.getShowHealthBars());
    graphics.setMinNumberEntity(menu.getMinNumberEntity());

    // Transmission de la nouvelle option
    graphics.setSameTypePeace(menu.getSameTypePeace());

    int nbGuerriers = menu.getNbGuerriers();

    (*parameters)["nb_guerriers"] = std::to_string(nbGuerriers);
    (*parameters)["mutation_type_rate"] = std::to_string(menu.getMutationTypeRate());
    (*parameters)["mutation_stats_rate"] = std::to_string(menu.getMutationStatsRate());
    (*parameters)["show_healthbar"] = std::to_string(menu.getShowHealthBars());
    (*parameters)["projectile_speed_multiplier_index"] = std::to_string(menu.getSpeedIndex());
    (*parameters)["music"] = std::to_string(menu.getMusicOn());
    (*parameters)["same_type_peace"] = std::to_string(menu.getSameTypePeace());
    (*parameters)["min_number_entity"] = std::to_string(menu.getMinNumberEntity());

    SDL_GetWindowSize(graphics.getWindow(), &winW, &winH);

    // --- NOUVEAU SPAWN ALEATOIRE ---
    std::vector<Entity*> entities;
    std::array<std::string, 4> types = {"Guerrier", "Archer", "Mage", "Golem"};

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

    if (menu.getMusicOn()) {
        Mix_HaltMusic();
        Mix_PlayMusic(graphics.getGameMusic(), -1);
    }

    graphics.startAllEntitiesThread();
    bool running = true;
    while (running) {
        graphics.update(&running, keep_playing);
        SDL_Delay(16);
    }
    //On récupère la taille de la fenêtre pour rouvrir la fenêtre du menu à la même taille
    SDL_GetWindowSize(graphics.getWindow(), &winW, &winH);
    (*parameters)["window_width"] = std::to_string(winW);
    (*parameters)["window_height"] = std::to_string(winH);
}

int main() {
    if (!init()) return 1;

    std::map<std::string, std::string> parameters = {
        { "nb_guerriers", "10" },
        {"mutation_type_rate", "15"},
        {"mutation_stats_rate", "15"},
        {"show_healthbar", "true"},
        {"projectile_speed_multiplier_index", "1"},
        {"music", "true"},
        {"same_type_peace", "false"},
        {"window_width", "0"},
        {"window_height", "0"},
        {"min_number_entity", "5"}
    };

    bool keep_playing = true;
    while (keep_playing) {
        start_game(&keep_playing, &parameters);
    }

    IMG_Quit(); TTF_Quit(); Mix_CloseAudio(); Mix_Quit(); SDL_Quit();
    return 0;
}