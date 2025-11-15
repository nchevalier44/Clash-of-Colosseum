#include "Fireball.h"
#include "Projectile.h"
#include "Entity.h"
#include <SDL_image.h>
#include <iostream>


Fireball::Fireball(SDL_Renderer* renderer, int damage, int range)
    : Weapon(damage, range)
{
    for (int i = 1; i <= 8; ++i) {
        std::string path = "../assets/Mage/FB_" + std::to_string(i) + ".png";
        SDL_Surface* surf = IMG_Load(path.c_str());
        if (!surf) {
            std::cerr << "Erreur chargement " << path << " : " << IMG_GetError() << std::endl;
            continue;
        }

        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        SDL_FreeSurface(surf);
        frames.push_back(tex);
    }

    if (frames.empty()) {
        std::cerr << "⚠️ Aucun sprite Fireball chargé !" << std::endl;
    }

    last_frame_time = SDL_GetTicks();
}



void Fireball::attack(Entity* entity, Entity* owner, std::vector<Projectile*>* projectiles, int origin_x, int origin_y) {
    float speed = 1.2f;
    int size = 20;
    Projectile* projectile = new Projectile(owner, this->damage, speed, size,
                                            origin_x, origin_y,
                                            entity->getX(), entity->getY(),
                                            15000, this->frames);
    projectiles->push_back(projectile);
}

Fireball::~Fireball() {
    for (auto tex : frames)
        SDL_DestroyTexture(tex);
}

