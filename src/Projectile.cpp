#include "Projectile.h"

Projectile::Projectile(int damage, int speed, int x, int y, int dest_x, int dest_y) : damage(damage), speed(speed), x(x), y(y){
    if(dest_x > x){
        dx = speed;
    } else{
        dx = -speed;
    }

    if(dest_y > y){
        dy = speed;
    } else{
        dy = -speed;
    }
}

void Projectile::move(){
    x += dx;
    y += dy;
}

void Projectile::draw(SDL_Renderer* renderer){
    SDL_Rect rect = {x, y, 5, 5};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}