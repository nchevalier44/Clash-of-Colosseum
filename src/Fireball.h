#ifndef CLASH_OF_COLOSSEUM_FIREBALL_H
#define CLASH_OF_COLOSSEUM_FIREBALL_H

#include <vector>
#include "Weapon.h"

class Projectile;

class Fireball : public Weapon {
public:
    Fireball(int damage=25, int range=150);

    void attack(Entity* target, Entity* owner, std::vector<Projectile*>* projectiles, int origin_x, int origin_y);
    void draw(int x, int y, SDL_Renderer* renderer) override;
    std::string type() override { return "Fireball"; };

};

#endif //CLASH_OF_COLOSSEUM_FIREBALL_H
