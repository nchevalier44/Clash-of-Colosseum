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

int main() {
    // --- Initialisations SDL (Rien ne change ici) ---
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

    std::srand(std::time(nullptr)); // Init random

    Graphics graphics;

    // --- CONFIGURATION DU MENU ---
    Menu menu(graphics.getRenderer());

    // Cette fonction bloque le jeu tant que le joueur n'a pas appuyé sur ENTRÉE
    menu.configureParameters();

    // --- TRANSMISSION DES OPTIONS ---
    // 1. On applique la vitesse des projectiles (variable statique)
    Projectile::globalSpeedMultiplier = menu.getProjectileSpeedMultiplier();

    // 2. On transmet les options génétiques et graphiques à Graphics
    graphics.setMutationRate(menu.getMutationRate());
    graphics.setShowHealthBars(menu.getShowHealthBars());

    // 3. On récupère le nombre d'entités choisi
    int nbGuerriers = menu.getNbGuerriers();

    std::cout << "---- Parametres choisis ----\n";
    std::cout << "Nb Entites : " << nbGuerriers << "\n";
    std::cout << "Taux Mutation : " << menu.getMutationRate() << "%\n";
    std::cout << "Vitesse Tirs : x" << menu.getProjectileSpeedMultiplier() << "\n";

    // --- CRÉATION DES ENTITÉS (Spawn Circulaire) ---
    std::vector<Entity*> entities;
    std::array<std::string, 4> types = {"Guerrier", "Archer", "Mage", "Golem"};

    for (int i = 0; i < nbGuerriers; i++) {
        // Paramètres de l'arène (Cercle) pour éviter de spawner dans les murs
        int centerX = 320;
        int centerY = 240;
        int maxRadius = 180; // Marge de sécurité par rapport au rayon de 225

        // Génération polaire
        float angle = (std::rand() % 360) * (3.14159f / 180.0f);
        float dist = std::rand() % maxRadius;

        int x = centerX + (int)(dist * std::cos(angle));
        int y = centerY + (int)(dist * std::sin(angle));

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

    // --- BOUCLE DE JEU
    Mix_HaltMusic();
    Mix_PlayMusic(graphics.getGameMusic(), -1);

    bool running = true;
    while (running) {
        graphics.update(&running);
        SDL_Delay(16); // ~60 FPS
    }

    // --- NETTOYAGE ---
    IMG_Quit();
    TTF_Quit();
    Mix_CloseAudio();
    Mix_Quit();
    SDL_Quit();
    return 0;
}