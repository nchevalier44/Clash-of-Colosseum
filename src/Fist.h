#ifndef CLASH_OF_COLOSSEUM_FIST_H
#define CLASH_OF_COLOSSEUM_FIST_H

#include "Weapon.h"

class Fist : public Weapon{
public:
    Fist(int damage=15, int range=15);
    std::string type() override { return "Fist"; }
};


#endif //CLASH_OF_COLOSSEUM_FIST_H
