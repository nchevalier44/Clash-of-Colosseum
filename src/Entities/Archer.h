#ifndef CLASH_OF_COLOSSEUM_ARCHER_H
#define CLASH_OF_COLOSSEUM_ARCHER_H

#include "Entity.h"

class Archer : public Entity {
public:
    Archer(float x, float y, SDL_Renderer* renderer);
    void loadSprites(SDL_Renderer* renderer) override;
    void updateAttackCooldown() override;
};


#endif //CLASH_OF_COLOSSEUM_ARCHER_H
