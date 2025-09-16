#ifndef CLASH_OF_COLOSSEUM_PROJECTILE_H
#define CLASH_OF_COLOSSEUM_PROJECTILE_H

#include "Weapon.h"

class Projectile {
public:
    Projectile(int damage, int speed, int x, int y, int dest_x, int dest_y);
    void move();
    void draw(SDL_Renderer* renderer);
private:
    int x;
    int y;
    int dx;
    int dy;
    int speed;
    int damage;
};


#endif //CLASH_OF_COLOSSEUM_PROJECTILE_H
