#include "Guerrier.h"
#include "../Weapons/Spear.h"

Guerrier::Guerrier(float x, float y, SDL_Renderer* renderer) : Entity(x, y, renderer){
    weapon = new Spear(randomRange(35, 45), randomRange(18, 25)); //dégat, range

    this->baseSpriteSize = 20.0f;
    setRandomSize(40, 50);

    foot_offset = 10;
    updateAttackCooldown();
    this->hp = this->max_hp = randomRange(130, 160);
    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f; //+-15%
    type = "Guerrier";
    loadSprites(renderer);
}

void Guerrier::updateAttackCooldown() {
    this->attack_cooldown = 500 + (weapon->getDamage() * 30);
}

void Guerrier::loadSprites(SDL_Renderer* renderer) {
    for (auto tex : frames)
        SDL_DestroyTexture(tex);
    frames.clear();

    string base_path = "../assets/Shieldmaiden/" + direction + "/" + state + "_";
    int frame_count = 0;

    if (state == "idle") frame_count = 4;
    else if (state == "run") frame_count = 6;
    else if (state == "attack") frame_count = 3;

    for (int i = 0; i < frame_count; ++i) {
        string path = base_path + to_string(i) + ".png";
        SDL_Surface* surface = IMG_Load(path.c_str());
        if (!surface) {
            cerr << "Erreur chargement sprite: " << path << " - " << IMG_GetError() << endl;
            continue;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_FreeSurface(surface);
        frames.push_back(texture);
    }

    current_frame = 0;
}
