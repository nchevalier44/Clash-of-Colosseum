#ifndef CLASH_OF_COLOSSEUM_WEAPON_H
#define CLASH_OF_COLOSSEUM_WEAPON_H

#include <SDL2/SDL.h>
#include <string>
#include <vector>

class Projectile;
class Entity;

class Weapon {
public:
    Weapon(int damage=1, int range=40);
    ~Weapon() = default;

    int getRange() const { return range; };
    void setRange(int new_range);
    int getDamage() const { return damage; };
    void setDamage(int new_damage){ damage = new_damage; };

    virtual void attack(Entity* target);
    virtual void attack(Entity* target, Entity* owner, std::vector<Projectile*>* projectiles, int origin_x, int origin_y){};
    virtual std::string type() { return "Weapon"; }

protected:
    int damage;
    int range;
    int min_range;
    int max_range;
};

#endif //CLASH_OF_COLOSSEUM_WEAPON_H
