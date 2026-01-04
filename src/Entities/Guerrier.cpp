#include "Guerrier.h"
#include "../Weapons/Spear.h"

Guerrier::Guerrier(float x, float y, SDL_Renderer* renderer) : Entity(x, y, renderer){
    base_sprite_size = 1.6f;
    ratio_hitbox_width = 0.328f; //Calculé à partir des sprites
    ratio_hitbox_height = 0.405f;
    setRandomSize(0.7, 1.1);

    float min_range = ratio_hitbox_height * size * 64;
    int range = randomRange(int(min_range), int(2*min_range));
    weapon = new Spear(randomRange(35, 45), range); //dégat, range

    updateAttackCooldown();
    this->hp = this->max_hp = randomRange(130, 160);
    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f - 0.01*size; //+-15%
    type = "Guerrier";
    frame_to_attack = 1;
    frame_delay += (size - 1.5) * 15;

    sprites_file_right = "../assets/Guerrier/guerrier_right.png";
    sprites_file_left = "../assets/Guerrier/guerrier_left.png";

    loadSprites(renderer);
}

void Guerrier::updateAttackCooldown() {
    this->attack_cooldown = 500 + (weapon->getDamage() * 30);
}