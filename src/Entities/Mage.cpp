#include "Mage.h"
#include "../Weapons/Fireball.h"

Mage::Mage(float x, float y, SDL_Renderer* renderer) : Entity(x, y, renderer){
    base_sprite_size = 1.5f;
    ratio_hitbox_width = 0.265f; //Calculé à partir des sprites
    ratio_hitbox_height = 0.485f;
    setRandomSize(0.8, 1.2);

    float min_range = ratio_hitbox_height * size * 64;
    int range = randomRange(int(1.5*min_range), int(5*min_range));

    weapon = new Fireball(renderer, randomRange(30, 40), range);

    updateAttackCooldown();

    // Vitesse : Variation standard +/- 15% avec malus pour les grandes tailles
    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f - 0.01*size;

    // PV faibles (50-70) comparé aux autres classes (ex: Guerrier ~145, Golem ~300)
    this->hp = this->max_hp = randomRange(50, 70);
    type = "Mage";

    // Synchronisation : le projectile part à la 5ème frame de l'animation d'attaque
    frame_to_attack = 5;
    // Inertie : ralentit l'animation des entités plus grandes
    frame_delay += (size - 1.5) * 15;

    sprites_file_right = "../assets/Mage/mage_right.png";
    sprites_file_left = "../assets/Mage/mage_left.png";

    loadSprites(renderer);
}

void Mage::updateAttackCooldown() {
    // Équilibrage : cadence de tir plus lente en fonction des dégats
    this->attack_cooldown = 500 + (weapon->getDamage() * 40);
}