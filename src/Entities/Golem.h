#ifndef CLASH_OF_COLOSSEUM_GOLEM_H
#define CLASH_OF_COLOSSEUM_GOLEM_H

#include "Entity.h"

class Golem : public Entity {
public:
    Golem(float x, float y, SDL_Renderer* renderer);
    void updateAttackCooldown() override;
};


#endif //CLASH_OF_COLOSSEUM_GOLEM_H
