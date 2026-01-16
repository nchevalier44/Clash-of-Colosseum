#include "Bow.h"
#include "Projectile.h"
#include "../Entities/Entity.h"
#include <SDL_image.h>
#include <iostream>

Bow::Bow(SDL_Renderer* renderer, int damage, int range) : Weapon(damage, range) {
    // Chargement de la texture "unique" de la flèche.
    // On la stocke dans 'frames' pour que tous les projectiles créés par cet arc partagent la même image en mémoire
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

Bow::~Bow() {
    for (auto tex : frames) {
        SDL_DestroyTexture(tex);
    }
    frames.clear();
}

// On instancie un objet "Projectile" qui aura sa propre vitesse et trajectoire.
void Bow::attack(Entity* entity, Entity* owner, std::vector<Projectile*>* projectiles, int origin_x, int origin_y) {
    float speed = 2.0f;

    // La taille du projectile est proportionnelle à celle du tireur
    int size = owner->getSize() * 25;

    // '15000' est la durée de vie (lifetime) en ms avant que la flèche disparaisse si elle ne touche rien.
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
        this->frames // On passe la texture chargée dans le constructeur
    );

    // Ajout au vecteur global géré par "Graphics" pour que la flèche soit mise à jour et dessinée à la frame suivante
    projectiles->push_back(projectile);
}