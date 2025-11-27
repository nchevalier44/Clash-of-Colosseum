#ifndef CLASH_OF_COLOSSEUM_GRAPHICS_H
#define CLASH_OF_COLOSSEUM_GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <vector>
#include <memory>
#include "Entity.h"
#include "Projectile.h"
#include "GameMenu.h"
#include <SDL_mixer.h>


class Graphics {
public:
    Graphics();
    ~Graphics();
    Mix_Music* getGameMusic() const { return gameMusic; }
    SDL_Renderer* getRenderer() { return renderer; } ;
    SDL_Window* getWindow() { return window; } ;
    void update(bool* running);
    SDL_Renderer* getRenderer() const;
    void deleteAllProjectiles();
    void handleEvent(bool* running);
    void updateEntities(bool draw);
    void updateProjectiles(bool draw);

    Entity* createNewEntityFromParents(Entity* e1, Entity* e2);

    void deleteEntity(Entity* entity);

    // Ajout de la méthode pour injecter les entités depuis le main
    void setEntities(const std::vector<Entity*>& ents);

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    std::vector<Entity*> entities;
    std::vector<Projectile*> projectiles;
    SDL_Texture* backgroundTexture;
    Mix_Music* gameMusic = nullptr;
    GameMenu* game_menu = nullptr;

};

#endif //CLASH_OF_COLOSSEUM_GRAPHICS_H
