#ifndef CLASH_OF_COLOSSEUM_WEAPON_H
#define CLASH_OF_COLOSSEUM_WEAPON_H

#include <SDL2/SDL.h>

class Entity;

class Weapon {
public:
    Weapon(int damage=1, int range=10);
    void draw(int x, int y, SDL_Renderer* renderer);

    int getRange() const { return range; };
    int getDamage() const { return damage; };

    virtual void attack(Entity* entity);

private:
    int damage;
    int range;
};


#endif //CLASH_OF_COLOSSEUM_WEAPON_H
