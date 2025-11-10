#ifndef CLASH_OF_COLOSSEUM_PROJECTILE_H
#define CLASH_OF_COLOSSEUM_PROJECTILE_H

#include "Weapon.h"

class Projectile {
public:
    Projectile(Entity* owner, int damage, float speed, int size, int x, int y, int dest_x, int dest_y, int max_alive_time=10);
    void move();
    void draw(SDL_Renderer* renderer);
    int getX() const { return x; };
    int getY() const { return y; };
    int getDamage() const { return damage; };
    Entity* getOwner() const { return owner; };
    Uint32 getEndTime() const { return end_time; }

private:
    float x;
    float y;
    float dx;
    float dy;
    int damage;
    Entity* owner = nullptr;
    int size=10;
protected:
    Uint32 end_time;
};


#endif //CLASH_OF_COLOSSEUM_PROJECTILE_H
