#ifndef CLASH_OF_COLOSSEUM_GRAPHICS_H
#define CLASH_OF_COLOSSEUM_GRAPHICS_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>
#include <vector>
#include "Entity.h"


class Graphics {
public:
    Graphics();
    ~Graphics();

    SDL_Renderer* getRenderer() { return renderer; } ;
    SDL_Window* getWindow() { return window; } ;
    void update(bool* running);
    SDL_Renderer* getRenderer() const;
    void setEntities(const std::vector<Entity*>& e) { entities = e; }


private:
    SDL_Window* window;
    SDL_Renderer* renderer;
    short* circle;
    std::vector<Entity*> entities;
};

#endif //CLASH_OF_COLOSSEUM_GRAPHICS_H
