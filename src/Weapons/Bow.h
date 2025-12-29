#ifndef CLASH_OF_COLOSSEUM_BOW_H
#define CLASH_OF_COLOSSEUM_BOW_H

#include <vector>
#include "Weapon.h"

class Projectile;

class Bow : public Weapon {
public:
    Bow(SDL_Renderer* renderer, int damage=25, int range=300);
    ~Bow();
    void attack(Entity* target, Entity* owner, std::vector<Projectile*>* projectiles, int origin_x, int origin_y);
    std::string type() override { return "Bow"; }
private:
    std::vector<SDL_Texture*> frames;
};


#endif //CLASH_OF_COLOSSEUM_BOW_H
