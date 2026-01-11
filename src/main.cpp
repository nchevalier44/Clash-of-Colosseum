#include "Graphics.h"
#include "Menu/SettingsMenu.h"
#include "Menu/GameMenu.h"
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

#include "SimulationStats.h"
#include "Menu/MainMenu.h"


bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) std::cerr << Mix_GetError() << std::endl;
    if (TTF_Init() != 0) return false;
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) return false;

    std::srand(std::time(nullptr));
    return true;
}

void start_game(bool* keep_playing, std::map<std::string, std::string>* parameters, SDL_Window* window, SDL_Renderer* renderer, std::vector<SimulationStats*>* simulations_stats) {
    Graphics graphics(window, renderer);

    MainMenu menu(window, renderer, &graphics, parameters, simulations_stats, keep_playing);

    if (!(*keep_playing)) return;

    SimulationStats* simulation_stats = new SimulationStats();
    simulations_stats->push_back(simulation_stats);
    graphics.setSimulationsStats(simulation_stats);
    simulation_stats->setStartTime(std::time(nullptr));

    int winW, winH;
    SDL_GetWindowSize(graphics.getWindow(), &winW, &winH);

    // --- NOUVEAU SPAWN ALEATOIRE ---
    std::vector<Entity*> entities;
    std::array<std::string, 4> types = {"Guerrier", "Archer", "Mage", "Golem"};

    int nbGuerriers = std::stoi((*parameters)["nb_guerriers"]);

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

    if ((*parameters)["music"] == "1") {
        Mix_HaltMusic();
        Mix_PlayMusic(graphics.getGameMusic(), -1);
    }

    graphics.startAllEntitiesThread();
    simulation_stats->addNewGeneration(entities, 1);
    bool running = true;
    while (running) {
        graphics.update(&running, keep_playing);
        SDL_Delay(16);
    }
    simulation_stats->setEndTime(std::time(nullptr));
}

int main() {
    if (!init()) return 1;

    std::map<std::string, std::string> parameters = {
        { "nb_guerriers", "10" },
        {"mutation_type_rate", "15"},
        {"mutation_stats_rate", "15"},
        {"show_healthbars", "1"},
        {"projectile_speed_multiplier_index", "1"},
        {"music", "1"},
        {"same_type_peace", "0"},
        {"min_number_entity", "5"}
    };
    std::vector<SimulationStats*> simulations_stats;

    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer);
    SDL_SetWindowTitle(window, "Clash of Colosseum");
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetWindowMinimumSize(window, 490, 450);

    bool keep_playing = true;
    while (keep_playing) {
        start_game(&keep_playing, &parameters, window, renderer, &simulations_stats);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit(); TTF_Quit(); Mix_CloseAudio(); Mix_Quit(); SDL_Quit();
    return 0;
}