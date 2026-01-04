#include "Bow.h"
#include "Projectile.h"
#include "../Entities/Entity.h"
#include <SDL_image.h>
#include <iostream>

// Constructeur : C'est ici qu'on charge l'image de la flèche !
Bow::Bow(SDL_Renderer* renderer, int damage, int range) : Weapon(damage, range) {
    std::string path = "../assets/Archer/arrow.png";
    SDL_Surface* surf = IMG_Load(path.c_str());
    min_range = 180;
    max_range = 220;

    if (!surf) {
        std::cerr << "Erreur chargement " << path << " : " << IMG_GetError() << std::endl;
    } else {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
        SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
        SDL_FreeSurface(surf);
        frames.push_back(tex);
    }
}

// Destructeur : On nettoie la mémoire
Bow::~Bow() {
    for (auto tex : frames) {
        SDL_DestroyTexture(tex);
    }
    frames.clear();
}

void Bow::attack(Entity* entity, Entity* owner, std::vector<Projectile*>* projectiles, int origin_x, int origin_y) {
    //launch a projectile in direction of the entity
    float speed = 2.0f;
    int size = owner->getSize() * 25; // Taille ajustée

    // Création du projectile avec les frames (la flèche)
    Projectile* projectile = new Projectile(
        owner,
        this->damage,
        speed,
        size,
        origin_x,
        origin_y,
        entity->getX(),
        entity->getY(),
        15000,
        this->frames
    );

    projectiles->push_back(projectile);
}