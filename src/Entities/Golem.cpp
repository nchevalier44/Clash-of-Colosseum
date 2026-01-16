#include "Golem.h"
#include "../Weapons/Punch.h"

Golem::Golem(float x, float y, SDL_Renderer* renderer) : Entity(x, y, renderer){
    // Taille de base beaucoup plus imposante que les autres unités (2.75x la normale)
    base_sprite_size = 2.75f;
    ratio_hitbox_width = 0.485; // Calculé à partir des sprites
    ratio_hitbox_height = 0.485;
    setRandomSize(0.9, 1.3);

    // Portée très courte (corps-à-corps) ajustée à sa hitbox massive
    float min_range = ratio_hitbox_height * size * 64;
    int range = randomRange(int(0.9*min_range), int(1.1*min_range));
    weapon = new Punch(randomRange(15, 25), range);

    // Vitesse : Base lente (0.8) + variation aléatoire + fort malus pour les très gros Golems
    this->move_speed *= 0.8f + randomRange(-15, 15) / 100.f - 0.01*size;

    updateAttackCooldown();

    // PV massifs (Rôle de Tank)
    this->hp = this->max_hp = randomRange(280, 350);
    type = "Golem";

    this->frame_to_attack = 6; // L'impact du coup de poing se fait à la 6ème frame
    frame_delay += (size - 1.5) * 15; // Animation ralentie par la taille (inertie)

    sprites_file_right = "../assets/Golem/golem_right.png";
    sprites_file_left = "../assets/Golem/golem_left.png";

    loadSprites(renderer);
}


void Golem::updateAttackCooldown() {
    // Équilibrage : Le Golem est lent à frapper (facteur 40 contre 30 pour l'Archer)
    this->attack_cooldown = 500 + (weapon->getDamage() * 40);
}