#include "Fireball.h"
#include "Projectile.h"
#include "Entity.h"

Fireball::Fireball(int damage, int range) : Weapon(damage, range){
}

void Fireball::attack(Entity* entity, Entity* owner, std::vector<Projectile*>* projectiles, int origin_x, int origin_y) {
    //launch a projectile in direction of the entity
    float speed = 1.2f;
    int size = 20;
    Projectile* projectile = new Projectile(owner, this->damage, speed, size, origin_x, origin_y, entity->getX(), entity->getY(), 15000);
    projectiles->push_back(projectile);
}

void Fireball::draw(int x, int y, SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // marron bois
    SDL_Rect rect = {x, y, 20, 5};
    SDL_RenderFillRect(renderer, &rect);
}