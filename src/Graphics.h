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
    static std::atomic<int> game_time_speed;
    static std::chrono::high_resolution_clock::time_point last_change_speed;
    static SimulationStats* simulations_stats;

    static void changeGameSpeed();

    Graphics(SDL_Window* window, SDL_Renderer* renderer);
    ~Graphics();
    Mix_Music* getGameMusic() const { return gameMusic; }
    SDL_Renderer* getRenderer() { return renderer; }
    SDL_Window* getWindow() { return window; }

    void update(bool* running, bool* keep_playing);

    bool multipleTypesAreAlive();

    void deleteDeadEntities();

    std::map<std::string, int> getNumberEntitiesPerTypes();

    SDL_Renderer* getRenderer() const;
    void deleteAllProjectiles();

    void increaseAllEntitiesAge();

    void handleEvent(bool* running, bool* keep_playing);

    Entity *getEntityAtPos(float x, float y);

    void updateEntities(bool draw);
    void updateProjectiles(bool draw);
    void setMutationTypeRate(int rate) { mutationTypeRate = rate; }
    void setMutationStatsRate(int rate) { mutationStatsRate = rate; }
    void setSameTypePeace(bool peace) { same_type_peace = peace; }
    void setShowHealthBars(bool show) { showHealthBars = show; }

    void startAllEntitiesThread();
    void stopAllEntitiesThread();

    void pauseAllEntities(bool pause);

    Entity* createNewEntityFromParents(Entity* e1, Entity* e2);

    void deleteEntity(Entity* entity);

    // Ajout de la méthode pour injecter les entités depuis le main
    void setEntities(const std::vector<Entity*>& ents);
    void setMinNumberEntity(int number){ min_number_entity = number; };

    std::mutex global_mutex;
    pair<std::string, int> winner = {"", 0};

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int mutationTypeRate = 15;
    int mutationStatsRate = 15;
    bool showHealthBars = true;
    bool same_type_peace = false;
    bool end_of_game = false;

    bool is_game_paused = false;
    Uint32 start_gen_time = 0;

    int min_number_entity = 5;
    std::vector<Entity*> entities;
    std::vector<Projectile*> projectiles;
    std::vector<Entity*> entities_to_delete;
    SDL_Texture* backgroundTexture;
    int generation = 1;
    Mix_Music* gameMusic = nullptr;
    GameMenu* game_menu = nullptr;


    Entity *instantiateChildByType(Entity *e1, Entity *e2);
    float calculateNewAttribute(float value1, float value2);

};

#endif //CLASH_OF_COLOSSEUM_GRAPHICS_H
