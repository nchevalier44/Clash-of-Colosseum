#ifndef CLASH_OF_COLOSSEUM_SPEAR_H
#define CLASH_OF_COLOSSEUM_SPEAR_H

#include "Weapon.h"

class Spear : public Weapon{
public:
    Spear(int damage=15, int range=15);
    std::string type() override { return "Spear"; }
};


#endif //CLASH_OF_COLOSSEUM_SPEAR_H
