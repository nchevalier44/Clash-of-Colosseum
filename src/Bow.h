#ifndef CLASH_OF_COLOSSEUM_BOW_H
#define CLASH_OF_COLOSSEUM_BOW_H

#include <vector>
#include "Weapon.h"

class Projectile;

class Bow : public Weapon {
public:
    Bow(int damage=25, int range=150);

    void attack(Entity* target, Entity* owner, std::vector<Projectile*>* projectiles, int origin_x, int origin_y);
    void draw(int x, int y, SDL_Renderer* renderer);
    std::string type() override { return "Bow"; };
};


#endif //CLASH_OF_COLOSSEUM_BOW_H
