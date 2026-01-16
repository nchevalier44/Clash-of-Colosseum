#include "Fireball.h"
#include "Projectile.h"
#include "../Entities/Entity.h"
#include <SDL_image.h>
#include <iostream>


Fireball::Fireball(SDL_Renderer* renderer, int damage, int range)
    : Weapon(damage, range){
    min_range = 130;
    max_range = 150;

    // Contrairement à l'arc qui a une image fixe, la boule de feu est animée.
    // On charge une séquence de 5 sprites (FB_moving_1 à 5) pour créer l'effet de flamme vacillante.
    for (int i = 1; i <= 5; ++i) {
        std::string path = "../assets/Mage/FB_moving_" + std::to_string(i) + ".png";
        SDL_Surface* surf = IMG_Load(path.c_str());
        if (!surf) {
            std::cerr << "Erreur chargement " << path << " : " << IMG_GetError() << std::endl;
            continue;
        }

        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        SDL_FreeSurface(surf);
        frames.push_back(tex); // On stocke toute l'animation dans le vecteur
    }

    if (frames.empty()) {
        std::cerr << "⚠️ Aucun sprite Fireball chargé !" << std::endl;
    }

    last_frame_time = SDL_GetTicks();
}

void Fireball::attack(Entity* entity, Entity* owner, std::vector<Projectile*>* projectiles, int origin_x, int origin_y) {
    // Équilibrage : Vitesse réduite (1.2f) comparée aux flèches (2.0f).
    // La magie fait plus mal ou a des effets de zone, mais voyage plus lentement.
    float speed = 1.2f;
    int size = owner->getSize() * 25;

    // Création du projectile en lui passant la liste complète des frames pour qu'il s'anime en vol
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