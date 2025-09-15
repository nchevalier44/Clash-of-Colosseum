#include "Entity.h"

Entity::Entity(int x, int y, int size, int hp) {
    this->hp = hp;
    this->size = size;
    this->x = x;
    this->y = y;
}

void Entity::drawHealthBar(SDL_Renderer* renderer) {
    SDL_Rect border = {x-25, y-50, size*3, size};
    SDL_Rect health = {x-25, y-50, (size*3)*hp/100, size};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &health);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &border);
}

void Entity::setSize(int new_size) {
    if (new_size >= 0) {
        size = new_size;
    } else {
        size = 0;
    }
}
void Entity::setHp(int new_hp){
    if(new_hp >=0){
        hp = new_hp;
    }  else{
        hp = 0;
    }
}