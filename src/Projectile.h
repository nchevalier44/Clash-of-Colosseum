#ifndef CLASH_OF_COLOSSEUM_PROJECTILE_H
#define CLASH_OF_COLOSSEUM_PROJECTILE_H

#include "Weapon.h"

class Projectile {
public:
    Projectile(Entity* owner, int damage, float speed, int x, int y, int dest_x, int dest_y);
    void move();
    void draw(SDL_Renderer* renderer);
    int getX() const { return x; };
    int getY() const { return y; };
    int getDamage() const { return damage; };
    Entity* getOwner() const { return owner; };

private:
    float x;
    float y;
    float dx;
    float dy;
    int damage;
    Entity* owner = nullptr;
};


#endif //CLASH_OF_COLOSSEUM_PROJECTILE_H
