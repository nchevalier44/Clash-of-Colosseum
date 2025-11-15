#ifndef CLASH_OF_COLOSSEUM_WEAPON_H
#define CLASH_OF_COLOSSEUM_WEAPON_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>

class Projectile;
class Entity;

class Weapon {
public:
    Weapon(int damage=1, int range=10);
    ~Weapon() = default;

    int getRange() const { return range; };
    int getDamage() const { return damage; };

    virtual void attack(Entity* target);
    virtual void attack(Entity* target, Entity* owner, std::vector<Projectile*>* projectiles, int origin_x, int origin_y){};
    virtual std::string type() { return "Weapon"; }

protected:
    int damage;
    int range;
};

#endif //CLASH_OF_COLOSSEUM_WEAPON_H
