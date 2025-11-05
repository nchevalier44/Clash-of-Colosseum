#include "Weapon.h"
#include "Entity.h"

Weapon::Weapon(int damage, int range) : damage(damage), range(range){
}

void Weapon::draw(int x, int y, SDL_Renderer* renderer){
    SDL_Rect rect = {x, y, 5, 20};
    SDL_SetRenderDrawColor(renderer, 153, 81, 43, 255);
    SDL_RenderFillRect(renderer, &rect);
}

void Weapon::attack(Entity* entity){
    entity->setHp(entity->getHp() - this->damage);
}

void Catapulte::draw(int x, int y, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); // gris pierre
    SDL_Rect rect = {x, y, 25, 25}; // gros bloc
    SDL_RenderFillRect(renderer, &rect);
}

void BouleDeFeu::draw(int x, int y, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 69, 0, 255); // rouge/orange feu
    SDL_Rect rect = {x, y, 15, 15}; // boule
    SDL_RenderFillRect(renderer, &rect);
}