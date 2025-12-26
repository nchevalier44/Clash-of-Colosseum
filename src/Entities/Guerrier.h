#ifndef CLASH_OF_COLOSSEUM_GUERRIER_H
#define CLASH_OF_COLOSSEUM_GUERRIER_H

#include "Entity.h"

class Guerrier : public Entity {
public:
    Guerrier(float x, float y, SDL_Renderer* renderer);
    void loadSprites(SDL_Renderer* renderer) override;
    void updateAttackCooldown() override;
};
#endif //CLASH_OF_COLOSSEUM_GUERRIER_H
