#ifndef CLASH_OF_COLOSSEUM_BOW_H
#define CLASH_OF_COLOSSEUM_BOW_H

#include "Weapon.h"

class Bow : public Weapon {
public:
    Bow(int damage=5, int range=10);

    void attack(Entity* entity) override;
};


#endif //CLASH_OF_COLOSSEUM_BOW_H
