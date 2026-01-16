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

// Initialisation de tous les sous-systèmes de la SDL (Vidéo, Audio, Police, Images)
bool init() {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) return false;
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) std::cerr << Mix_GetError() << std::endl;
    if (TTF_Init() != 0) return false;
    int imgFlags = IMG_INIT_PNG | IMG_INIT_JPG;
    if ((IMG_Init(imgFlags) & imgFlags) != imgFlags) return false;

    std::srand(std::time(nullptr));
    return true;
}

// Lance une session de simulation
// Cette fonction est appelée en boucle tant que l'utilisateur ne quitte pas le jeu
void start_game(bool* keep_playing, std::map<std::string, std::string>* parameters, SDL_Window* window, SDL_Renderer* renderer, std::vector<SimulationStats*>* simulations_stats) {
    Graphics graphics(window, renderer);

    // Affiche le Menu Principal et bloque l'exécution ici tant que l'utilisateur n'a pas cliqué sur "Jouer" ou "Quitter"
    MainMenu menu(window, renderer, &graphics, parameters, simulations_stats, keep_playing);

    // Si l'utilisateur a cliqué sur "Quitter" dans le menu, on sort de la fonction immédiatement
    if (!(*keep_playing)) return;

    // Création de l'objet de statistiques pour cette session spécifique
    SimulationStats* simulation_stats = new SimulationStats();
    simulations_stats->push_back(simulation_stats);
    Graphics::simulations_stats = simulation_stats;
    simulation_stats->setStartTime(std::time(nullptr));

    // Application des paramètres configurés dans le SettingsMenu
    simulation_stats->setSameTypePeace((*parameters)["same_type_peace"] == "1");
    simulation_stats->setMutationStatsRate(std::stoi((*parameters)["mutation_stats_rate"]));
    simulation_stats->setMutationTypeRate(std::stoi((*parameters)["mutation_type_rate"]));

    int winW, winH;
    SDL_GetWindowSize(graphics.getWindow(), &winW, &winH);

    // Génération de la première population
    std::vector<Entity*> entities;
    std::array<std::string, 4> types = {"Guerrier", "Archer", "Mage", "Golem"};

    int nbGuerriers = std::stoi((*parameters)["nb_guerriers"]);

    for (int i = 0; i < nbGuerriers; i++) {
        // Spawn aléatoire sur tous l'écran avec une marge de sécurité (30px)
        int x = std::rand() % (winW - 60) + 30;
        int y = std::rand() % (winH - 60) + 30;

        // Choix aléatoire de la classe
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
        Mix_PlayMusic(graphics.getGameMusic(), -1); // -1 = boucle infinie
    }

    graphics.startAllEntitiesThread();

    // Enregistrement des infos de la 1ere génération (hp, damage, ...)
    simulation_stats->addNewGeneration(entities, 1);
    Graphics::last_change_speed = std::chrono::high_resolution_clock::now();

    // Boucle de Jeu
    bool running = true;
    while (running) {
        //running deviendra false quand l'utilisateur arretera la simulation
        //keep_playing deviendra false si l'utilisateur clique sur la croix en haut à droite pour quitter entièrement le jeu
        graphics.update(&running, keep_playing);
        SDL_Delay(16); // 60 FPS
    }
    simulation_stats->setEndTime(std::time(nullptr));
    Graphics::game_time_speed = 1; // Reset de la vitesse pour la prochaine partie
}

int main() {
    if (!init()) return 1;

    // Paramètres par défaut (Modifiables via SettingsMenu)
    std::map<std::string, std::string> parameters = {
        { "nb_guerriers", "10" },
        {"mutation_type_rate", "15"},
        {"mutation_stats_rate", "15"},
        {"show_healthbars", "1"},
        {"projectile_speed_multiplier_index", "1"},
        {"music", "1"},
        {"same_type_peace", "0"}, // 0 = Chacun pour soi, 1 = Paix raciale
        {"min_number_entity", "5"} // Seuil de déclenchement de la prochaine génération
    };

    // Stocke l'historique de toutes les parties jouées depuis le lancement du programme
    std::vector<SimulationStats*> simulations_stats;

    SDL_Window* window;
    SDL_Renderer* renderer;
    // Création de la fenêtre (640x480 par défaut, mais redimensionnable)
    SDL_CreateWindowAndRenderer(640, 480, SDL_WINDOW_RESIZABLE, &window, &renderer);
    SDL_SetWindowTitle(window, "Clash of Colosseum");
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND); // Active la transparence alpha
    SDL_SetWindowMinimumSize(window, 490, 450); // Empêche de trop réduire la fenêtre (casse l'UI)

    bool keep_playing = true;

    // Permet de revenir au menu principal après une partie et d'en relancer une par la suite
    while (keep_playing) {
        start_game(&keep_playing, &parameters, window, renderer, &simulations_stats);
    }

    // Nettoyage final
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    IMG_Quit(); TTF_Quit(); Mix_CloseAudio(); Mix_Quit(); SDL_Quit();
    return 0;
}