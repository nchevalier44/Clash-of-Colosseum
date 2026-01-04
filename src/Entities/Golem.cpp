#include "Golem.h"
#include "../Weapons/Fist.h"

Golem::Golem(float x, float y, SDL_Renderer* renderer) : Entity(x, y, renderer){
    base_sprite_size = 2.75f;
    ratio_hitbox_width = 0.485; //Calculé à partir des sprites
    ratio_hitbox_height = 0.485;
    setRandomSize(0.9, 1.3);

    float min_range = ratio_hitbox_height * size * 64;
    int range = randomRange(int(0.9*min_range), int(1.1*min_range));
    weapon = new Fist(randomRange(15, 25), range);

    this->move_speed *= 0.8f + randomRange(-15, 15) / 100.f; //+-15%
    updateAttackCooldown();
    this->hp = this->max_hp = randomRange(280, 350);
    type = "Tank";
    this->frame_delay = 150;
    this->frame_to_attack = 6;

    sprites_file_right = "../assets/Golem/golem_right.png";
    sprites_file_left = "../assets/Golem/golem_left.png";

    loadSprites(renderer);
}


void Golem::updateAttackCooldown() {
    this->attack_cooldown = 500 + (weapon->getDamage() * 40);
}