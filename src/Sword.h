#ifndef CLASH_OF_COLOSSEUM_SWORD_H
#define CLASH_OF_COLOSSEUM_SWORD_H

#include "Weapon.h"

class Sword : public Weapon{
public:
    Sword(int damage=15, int range=10);
    void draw(int x, int y, SDL_Renderer* renderer) override;
    std::string type() override { return "Sword"; }
};


#endif //CLASH_OF_COLOSSEUM_SWORD_H
