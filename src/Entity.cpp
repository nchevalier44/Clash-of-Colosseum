#include "Entity.h"
#include "Sword.h"
#include "Bow.h"
#include <iostream>

Entity::Entity(int x, int y, int size, int hp, int max_hp) {
    this->hp = hp;
    this->max_hp = max_hp;
    this->size = size;
    this->x = x;
    this->y = y;

    //A Changer :
    weapon = new Sword(10, 10);
}

bool Entity::canAttack(Entity* entity) {
    return this->distance(entity->getX(), entity->getY()) < entity->getWeapon()->getRange();
}

double Entity::distance(int x2, int y2){
    return sqrt(pow(x2-x, 2)+pow(y2-y, 2));
}

Entity* Entity::findClosestEntity(std::vector<Entity*> entities){
    if (entities.size() <= 1) {
        return nullptr;
    }

    Entity* closest_entity = (entities[0] == this ? entities[1] : entities[0]);
    double dist = distance(closest_entity->getX(), closest_entity->getY());

    for(Entity* e : entities){
        if(e != this){
            double d = distance(e->getX(), e->getY());
            if(d < dist){
                closest_entity = e;
                dist = d;
            }
        }
    }

    if(closest_entity == this){
        return nullptr;
    }

    return closest_entity;
}

void Entity::drawHealthBar(SDL_Renderer* renderer) {
    SDL_Rect border = {x-25, y-50, size*3, size};
    SDL_Rect health = {x-25, y-50, (size*3)*hp/max_hp, size};
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
    if(new_hp > 0){
        hp = new_hp;
    }  else{
        hp = 0;
    }
}

void Entity::moveInDirection(int x, int y){
    if(x > this->x){
        this->x += 1;
    } else if(x < this->x){
        this->x -= 1;
    }

    if(y > this->y){
        this->y += 1;
    } else if(y < this->y){
        this->y -= 1;
    }
}