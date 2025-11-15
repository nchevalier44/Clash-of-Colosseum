#ifndef CLASH_OF_COLOSSEUM_FIREBALL_H
#define CLASH_OF_COLOSSEUM_FIREBALL_H

#include <vector>
#include "Weapon.h"

class Projectile;

class Fireball : public Weapon {
public:
    Fireball(SDL_Renderer* renderer, int damage=25, int range=150);
    ~Fireball();
    void attack(Entity* target, Entity* owner, std::vector<Projectile*>* projectiles, int origin_x, int origin_y);
    std::string type() override { return "Fireball"; };
private:
    std::vector<SDL_Texture*> frames;
    int current_frame = 0;
    Uint32 last_frame_time = 0;
    Uint32 frame_delay = 80; // ms entre frames

};

#endif //CLASH_OF_COLOSSEUM_FIREBALL_H
