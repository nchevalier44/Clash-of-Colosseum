#include <iostream>
#include "Projectile.h"
#include <iostream>

Projectile::Projectile(int damage, float speed, int x, int y, int dest_x, int dest_y) : damage(damage), x(x), y(y){
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
    std::cout << "DX : " << dx << std::endl;
    std::cout << "DY : " << dy << std::endl;
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