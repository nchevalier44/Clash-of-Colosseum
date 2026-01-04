#include "Archer.h"
#include "../Weapons/Bow.h"

Archer::Archer(float x, float y, SDL_Renderer* renderer) : Entity(x, y, renderer){
    base_sprite_size = 1.15f;
    ratio_hitbox_width = 0.5f;
    ratio_hitbox_height = 0.67f;
    setRandomSize(0.9, 1.1);

    float min_range = ratio_hitbox_height * size * 64;
    int range = randomRange(int(2*min_range), int(7*min_range));
    weapon = new Bow(renderer, randomRange(20, 30), range);

    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.0f - 0.01*size; //+-15%
    this->hp = this->max_hp = randomRange(70, 90);
    type = "Archer";
    frame_to_attack = 9;
    frame_delay += (size - 1.5) * 15;

    sprites_file_right = "../assets/Archer/archer_right.png";
    sprites_file_left = "../assets/Archer/archer_left.png";

    loadSprites(renderer);
}

void Archer::updateAttackCooldown() {
    this->attack_cooldown = 500 + (weapon->getDamage() * 30);
}