#ifndef CLASH_OF_COLOSSEUM_MAGE_H
#define CLASH_OF_COLOSSEUM_MAGE_H

#include "Entity.h"

class Mage : public Entity {
public:
    Mage(float x, float y, SDL_Renderer* renderer);
    void loadSprites(SDL_Renderer* renderer) override;
    void updateAttackCooldown() override;
};


#endif //CLASH_OF_COLOSSEUM_MAGE_H
