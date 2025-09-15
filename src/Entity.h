#ifndef CLASH_OF_COLOSSEUM_ENTITY_H
#define CLASH_OF_COLOSSEUM_ENTITY_H

#include <SDL2/SDL.h>
#include "Weapon.h"
#include <vector>


class Entity {
public:
    Entity(int x, int y, int size=10, int hp=100, int max_hp=100);

    void setX(int new_x){ x = new_x; };
    void setY(int new_y){ y = new_y; };
    void setHp(int new_hp);
    void setSize(int new_size);
    int getX() const { return x; };
    int getY() const { return y; };
    int getSize() const { return size; };
    int getHp() const { return hp; }

    double distance(int x2, int y2);
    Entity* findClosestEntity(std::vector<Entity*> entities);
    void move(int x, int y);
    void drawHealthBar(SDL_Renderer* renderer);

private:
    int hp;
    int max_hp;
    int size;
    int x;
    int y;
    Weapon weapon;
};


#endif //CLASH_OF_COLOSSEUM_ENTITY_H
