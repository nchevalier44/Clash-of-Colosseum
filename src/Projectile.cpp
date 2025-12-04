#include "Projectile.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float Projectile::globalSpeedMultiplier = 1.0f;

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

        // --- CALCUL DE L'ANGLE ---
        // atan2 renvoie l'angle en radians, on convertit en degrés pour SDL.
        // On suppose que le sprite de base (flèche) pointe vers la droite (0°).
        angle = std::atan2(dy, dx) * (180.0 / M_PI);
    } else {
        dx = dy = 0;
        angle = 0.0;
    }

    end_time = SDL_GetTicks() + max_alive_time;
    last_frame_time = SDL_GetTicks();
}


void Projectile::move(){
    x += dx * globalSpeedMultiplier;
    y += dy * globalSpeedMultiplier;
}

void Projectile::draw(SDL_Renderer* renderer, int time_speed) {
    if (!frames.empty()) {
        Uint32 now = SDL_GetTicks();
        if (now - last_frame_time > frame_delay / time_speed) {
            current_frame = (current_frame + 1) % frames.size();
            last_frame_time = now;
        }

        // --- CORRECTION PROPORTIONS ---
        int texW = 0, texH = 0;
        // On demande à SDL la taille réelle de l'image chargée
        SDL_QueryTexture(frames[current_frame], nullptr, nullptr, &texW, &texH);

        // On calcule le ratio (ex: une flèche est plus large que haute)
        float ratio = (float)texH / (float)texW;

        // On définit la largeur voulue (basée sur 'size')
        int draw_width = size * 3; // On allonge un peu pour que la flèche soit visible
        int draw_height = draw_width * ratio; // La hauteur s'adapte automatiquement !

        // On centre l'image
        SDL_Rect dest = {
            int(x) - draw_width / 2,
            int(y) - draw_height / 2,
            draw_width,
            draw_height
        };

        // Rendu avec rotation
        SDL_RenderCopyEx(renderer,
                         frames[current_frame],
                         nullptr,
                         &dest,
                         angle,
                         nullptr,
                         SDL_FLIP_NONE);
    } else {
        // Fallback carré blanc
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect rect = { int(x), int(y), size, size };
        SDL_RenderFillRect(renderer, &rect);
    }
}

Projectile::~Projectile() {
    // Ne pas détruire les textures ici, elles appartiennent à l'arme (Bow/Fireball)
}