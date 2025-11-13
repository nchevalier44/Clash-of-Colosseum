#ifndef CLASH_OF_COLOSSEUM_PROJECTILE_H
#define CLASH_OF_COLOSSEUM_PROJECTILE_H

#include "Weapon.h"
#include <SDL2/SDL.h>
#include <vector>

class Projectile {
public:
    Projectile(Entity* owner, int damage, float speed, int size,
               int x, int y, int dest_x, int dest_y,
               int max_alive_time = 10,
               std::vector<SDL_Texture*> frames = {});

    void move();
    void draw(SDL_Renderer* renderer);
    int getX() const { return x; };
    int getY() const { return y; };
    int getDamage() const { return damage; };
    Entity* getOwner() const { return owner; };
    Uint32 getEndTime() const { return end_time; }

private:
    float x, y, dx, dy;
    int damage;
    Entity* owner = nullptr;
    int size = 10;
    Uint32 end_time;

    std::vector<SDL_Texture*> frames;
    int current_frame = 0;
    Uint32 last_frame_time = 0;
    Uint32 frame_delay = 80;
};

#endif
