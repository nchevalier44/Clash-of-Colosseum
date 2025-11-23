#include "Projectile.h"
#include <cmath>

Projectile::Projectile(Entity* owner, int damage, float speed, int size,
                       int x, int y, int dest_x, int dest_y, int max_alive_time,
                       std::vector<SDL_Texture*> frames)
    : owner(owner), damage(damage), x(x), y(y), size(size), frames(frames)
{
    float dx_total = dest_x - x;
    float dy_total = dest_y - y;
    float length = std::sqrt(dx_total * dx_total + dy_total * dy_total);

    if (length != 0.0f) {
        dx = dx_total / length * speed;
        dy = dy_total / length * speed;
    } else {
        dx = dy = 0;
    }

    end_time = SDL_GetTicks() + max_alive_time;
    last_frame_time = SDL_GetTicks();
}


void Projectile::move(){
    x += dx;
    y += dy;
}

void Projectile::draw(SDL_Renderer* renderer, int time_speed) {
    if (!frames.empty()) {
        Uint32 now = SDL_GetTicks();
        if (now - last_frame_time > frame_delay / time_speed) {
            current_frame = (current_frame + 1) % frames.size();
            last_frame_time = now;
        }

        SDL_Rect dest = { int(x) - size / 2, int(y) - size / 2, size * 2, size * 2 };
        SDL_RenderCopy(renderer, frames[current_frame], nullptr, &dest);
    } else {
        // fallback : carré blanc si aucune frame
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect rect = { int(x), int(y), size, size };
        SDL_RenderFillRect(renderer, &rect);
    }
}
