#ifndef CLASH_OF_COLOSSEUM_ENTITY_H
#define CLASH_OF_COLOSSEUM_ENTITY_H

#include <SDL2/SDL.h>
#include <vector>

#include "Weapon.h"

class Entity {
public:
    Entity(int x, int y, int size=10, int hp=100, int max_hp=100);
    virtual ~Entity();

    void setX(int new_x){ x = new_x; };
    void setY(int new_y){ y = new_y; };
    void setHp(int new_hp);
    void setSize(int new_size);
    int getX() const { return x; };
    int getY() const { return y; };
    int getSize() const { return size; };
    int getHp() const { return hp; }
    Weapon* getWeapon() { return weapon; }

    bool canAttack(Entity* entity);

    double distance(int x2, int y2);
    Entity* findClosestEntity(std::vector<Entity*> entities);
    void moveInDirection(int x, int y);
    void drawHealthBar(SDL_Renderer* renderer);

protected:
    int hp;
    int max_hp;
    int size;
    int x;
    int y;
    Weapon* weapon;
};

class Guerrier : public Entity {
public:
    Guerrier(int x, int y, int size=15, int max_hp=120);
};

class Archer : public Entity {
public:
    Archer(int x, int y, int size=12, int max_hp=80);
};

class Tank : public Entity {
public:
    Tank(int x, int y, int size=25, int max_hp=200);
};

class Mage : public Entity {
public:
    Mage(int x, int y, int size=14, int max_hp=90);
};


#endif //CLASH_OF_COLOSSEUM_ENTITY_H
