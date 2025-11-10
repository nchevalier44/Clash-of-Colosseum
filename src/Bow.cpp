#include "Bow.h"
#include "Projectile.h"
#include "Entity.h"

Bow::Bow(int damage, int range) : Weapon(damage, range){
}

void Bow::attack(Entity* entity, Entity* owner, std::vector<Projectile*>* projectiles, int origin_x, int origin_y) {
    //launch a projectile in direction of the entity
    float speed = 1.5f;
    Projectile* projectile = new Projectile(owner, this->damage, speed, origin_x, origin_y, entity->getX(), entity->getY());
    projectiles->push_back(projectile);
}

void Bow::draw(int x, int y, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // marron bois
    SDL_Rect rect = {x, y, 20, 5};
    SDL_RenderFillRect(renderer, &rect);
}