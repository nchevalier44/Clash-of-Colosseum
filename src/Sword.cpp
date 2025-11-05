#include "Sword.h"

Sword::Sword(int damage, int range) : Weapon(damage, range){
    //pass
}

void Sword::draw(int x, int y, SDL_Renderer* renderer) {
    SDL_Rect rect = {x, y, 5, 30};
    SDL_SetRenderDrawColor(renderer, 192, 192, 192, 255); // gris acier
    SDL_RenderFillRect(renderer, &rect);
}