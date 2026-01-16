#include "Projectile.h"
#include <cmath>
#include "../Graphics.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float Projectile::globalSpeedMultiplier = 1.0f;

Projectile::Projectile(Entity* owner, int damage, float speed, int size,
                       int x, int y, int dest_x, int dest_y, int max_alive_time,
                       std::vector<SDL_Texture*> frames)
    : owner(owner), damage(damage), x(x), y(y), size(size), frames(frames)
{
    // Calcul du vecteur directeur (Distance totale X et Y)
    float dx_total = dest_x - x;
    float dy_total = dest_y - y;
    float length = std::sqrt(dx_total * dx_total + dy_total * dy_total);

    if (length != 0.0f) {
        // Normalisation du vecteur : on le ramène à une longueur de 1, puis on multiplie par la vitesse.
        // Cela assure que le projectile voyage à la vitesse 'speed', quelle que soit la distance de la cible.
        dx = dx_total / length * speed;
        dy = dy_total / length * speed;

        // atan2(y, x) renvoie l'angle en radians entre l'axe X et le point.
        // On convertit en degrés (180/PI) car SDL_RenderCopyEx attend des degrés.
        // Cela permet à la flèche de "pointer" vers sa destination.
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

void Projectile::draw(SDL_Renderer* renderer) {
    if (!frames.empty()) {
        // Gestion de l'animation (ex: boule de feu)
        Uint32 now = SDL_GetTicks();
        if (now - last_frame_time > frame_delay / Graphics::game_time_speed) {
            current_frame = (current_frame + 1) % frames.size();
            last_frame_time = now;
        }

        // On récupère la taille réelle de l'image (texture) pour calculer son ratio largeur/hauteur.
        int texW = 0, texH = 0;
        SDL_QueryTexture(frames[current_frame], nullptr, nullptr, &texW, &texH);

        float ratio = (float)texH / (float)texW;

        // On définit la largeur voulue, et la hauteur s'adapte automatiquement.
        // si on forçait un carré (size x size),la flèche serait écrasée et moche.
        int draw_width = size;
        int draw_height = draw_width * ratio;

        // On centre l'image
        SDL_Rect dest = {
            int(x - draw_width / 2),
            int(y - draw_height / 2),
            draw_width,
            draw_height
        };

        // Rendu avancé avec rotation (angle)
        SDL_RenderCopyEx(renderer,
                         frames[current_frame],
                         nullptr,
                         &dest,
                         angle,
                         nullptr, // Centre de rotation (nullptr = centre de l'image)
                         SDL_FLIP_NONE);
    } else {
        // Fallback : Carré blanc si l'image n'est pas chargée
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_Rect rect = { int(x), int(y), size, size };
        SDL_RenderFillRect(renderer, &rect);
    }
}

Projectile::~Projectile() {
    // Ne pas détruire les textures ici, elles appartiennent à l'arme (Bow/Fireball)
}