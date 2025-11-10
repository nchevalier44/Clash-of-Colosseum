#include "Projectile.h"

Projectile::Projectile(Entity* owner, int damage, float speed, int size, int x, int y, int dest_x, int dest_y) : owner(owner), damage(damage), x(x), y(y), size(size){
    float dx_total = dest_x - x;
    float dy_total = dest_y - y;
    float length = std::sqrt(dx_total * dx_total + dy_total * dy_total);

    if (length != 0.0f) {
        dx = dx_total / length * speed ;
        dy = dy_total / length * speed ;
    } else {
        dx = 0;
        dy = 0;
    }
    end_time = SDL_GetTicks() + 10 * 1000; //Destroy it-self in 10s
}

void Projectile::move(){
    x += dx;
    y += dy;
}

void Projectile::draw(SDL_Renderer* renderer){
    SDL_Rect rect = {int(this->x), int(this->y), size, size};
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}