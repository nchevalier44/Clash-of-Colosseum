#ifndef CLASH_OF_COLOSSEUM_FIST_H
#define CLASH_OF_COLOSSEUM_FIST_H

#include "Weapon.h"

class Punch : public Weapon{
public:
    Punch(int damage=15, int range=15);
    std::string type() override { return "Punch"; }
};


#endif //CLASH_OF_COLOSSEUM_FIST_H
