#ifndef CLASH_OF_COLOSSEUM_GRAPHICS_H
#define CLASH_OF_COLOSSEUM_GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <vector>
#include <memory>
#include "Entity.h"
#include "Projectile.h"


class Graphics {
public:
    Graphics();
    ~Graphics();

    SDL_Renderer* getRenderer() { return renderer; } ;
    SDL_Window* getWindow() { return window; } ;
    void update(bool* running);
    SDL_Renderer* getRenderer() const;

    void deleteEntity(Entity* entity);

    // Ajout de la méthode pour injecter les entités depuis le main
    void setEntities(const std::vector<std::unique_ptr<Entity>>& ents);

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // On ne stocke que des pointeurs bruts vers les entités créées dans main
    std::vector<Entity*> entities;
    std::vector<Projectile*> projectiles;
};

#endif //CLASH_OF_COLOSSEUM_GRAPHICS_H
