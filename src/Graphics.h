#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <SDL.h>
#include <vector>
#include <memory>
#include "Entity.h"
#include <SDL2/SDL2_gfxPrimitives.h>

class Graphics {
public:
    Graphics();
    ~Graphics();

    SDL_Renderer* getRenderer() const;
    void update(bool* running);

    // Ajout de la méthode pour injecter les entités depuis le main
    void setEntities(const std::vector<std::unique_ptr<Entity>>& ents);

private:
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    // On ne stocke que des pointeurs bruts vers les entités créées dans main
    std::vector<Entity*> entities;
};

#endif // GRAPHICS_H
