#include "Bow.h"
#include "Projectile.h"
#include "Entity.h"
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
    float speed = 1.75f;
    int size = 10; // Taille ajustée

    // Position de départ
    int launch_x = origin_x;
    int launch_y = origin_y;

    // Récupération de la direction (nécessite la modif dans Entity.h)
    std::string dir = owner->getDirection();

    // Décalage pour faire partir la flèche de l'arc et non des pieds
    int offset_x = 25;
    int offset_y = 25;

    if (dir == "right") {
        launch_x += offset_x;
        launch_y -= offset_y;
    } else {
        launch_x -= offset_x;
        launch_y -= offset_y;
    }

    // Création du projectile avec les frames (la flèche)
    Projectile* projectile = new Projectile(
        owner,
        this->damage,
        speed,
        size,
        launch_x,
        launch_y,
        entity->getX(),
        entity->getY(),
        15000,
        this->frames
    );

    projectiles->push_back(projectile);
}

// La méthode draw reste vide car l'arc est sur le sprite du personnage
void Bow::draw(int x, int y, SDL_Renderer* renderer) {
}