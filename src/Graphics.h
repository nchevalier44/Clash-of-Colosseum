#ifndef CLASH_OF_COLOSSEUM_GRAPHICS_H
#define CLASH_OF_COLOSSEUM_GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <vector>
#include <memory>
#include "Entities/Entity.h"
#include "Weapons/Projectile.h"
#include "Menu/GameMenu.h"
#include <SDL_mixer.h>
#include <mutex>

#include "SimulationStats.h"


class Graphics {
public:

    // atomic car c'est lu par beaucoup de threads en même temps
    static std::atomic<int> game_time_speed;
    static std::chrono::high_resolution_clock::time_point last_change_speed;
    static SimulationStats* simulations_stats;

    static void changeGameSpeed();

    Graphics(SDL_Window* window, SDL_Renderer* renderer);
    ~Graphics();

    Mix_Music* getGameMusic() const { return gameMusic; }
    SDL_Renderer* getRenderer() { return renderer; }
    SDL_Window* getWindow() { return window; }

    // Fonction appelé depuis la boucle principal du jeu
    void update(bool* running, bool* keep_playing);

    // Vérifie s'il reste au moins 2 espèces différentes (condition d'arrêt de la simulation quand same_type_peace est activé)
    bool multipleTypesAreAlive();

    void deleteDeadEntities();

    std::map<std::string, int> getNumberEntitiesPerTypes();

    SDL_Renderer* getRenderer() const;
    void deleteAllProjectiles();

    void increaseAllEntitiesAge();

    void handleEvent(bool* running, bool* keep_playing);

    Entity* getEntityAtPos(float x, float y);

    void updateEntities(bool draw);
    void updateProjectiles(bool draw);

    void setMutationTypeRate(int rate) { mutationTypeRate = rate; }
    void setMutationStatsRate(int rate) { mutationStatsRate = rate; }
    void setSameTypePeace(bool peace) { same_type_peace = peace; }
    void setShowHealthBars(bool show) { showHealthBars = show; }

    void startAllEntitiesThread();
    void stopAllEntitiesThread();

    // Met en pause le thread sans l'arreter complètement
    void pauseAllEntities(bool pause);

    // Crée une nouvelle entité basée sur les stats combinées de e1 et e2 + mutations
    Entity* createNewEntityFromParents(Entity* e1, Entity* e2);

    void deleteEntity(Entity* entity);

    // Injection des entités initiales depuis le main.cpp
    void setEntities(const std::vector<Entity*>& ents);
    void setMinNumberEntity(int number){ min_number_entity = number; };

    // Sécurise l'accès à entities ou projectiles
    std::mutex global_mutex;

    pair<std::string, int> winner = {"", 0};

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // Taux de mutation (%)
    int mutationTypeRate = 15;
    int mutationStatsRate = 15;

    bool showHealthBars = true;
    bool same_type_peace = false; // Si true, les entités de même espèce ne s'attaquent pas
    bool end_of_game = false;

    bool is_game_paused = false;
    Uint32 start_gen_time = 0;

    int min_number_entity = 5; // Seuil de population déclenchant la reproduction

    std::vector<Entity*> entities;
    std::vector<Projectile*> projectiles;
    std::vector<Entity*> entities_to_delete;

    SDL_Texture* backgroundTexture;
    int generation = 1;
    Mix_Music* gameMusic = nullptr;
    GameMenu* game_menu = nullptr;

    // Création de la nouvelle entité enfant à partir des parents
    Entity* instantiateChildByType(Entity* e1, Entity* e2);
    float calculateNewAttribute(float value1, float value2);

};

#endif //CLASH_OF_COLOSSEUM_GRAPHICS_H