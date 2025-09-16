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
