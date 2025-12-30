#include "Mage.h"
#include "../Weapons/Fireball.h"

Mage::Mage(float x, float y, SDL_Renderer* renderer) : Entity(x, y, renderer){
    weapon = new Fireball(renderer, randomRange(30, 40), randomRange(130, 150));
    foot_offset = 100;
    this->baseSpriteSize = 5.0f;
    setRandomSize(30, 35);
    updateAttackCooldown();
    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f; //+-15%
    this->hp = this->max_hp = randomRange(50, 70);
    type = "Mage";

    loadSprites(renderer);
}

void Mage::updateAttackCooldown() {
    this->attack_cooldown = 500 + (weapon->getDamage() * 40);
}

void Mage::loadSprites(SDL_Renderer* renderer) {
    for (auto tex : frames)
        SDL_DestroyTexture(tex);
    frames.clear();

    std::string base_path = "../assets/Mage/doctor_";
    std::string file_path;
    int frame_count = 0;

    if (state == "idle") {
        file_path = base_path + "idle.png";
        frame_count = 4;
    }
    else if (state == "run") {
        file_path = base_path + "move.png";
        frame_count = 6;
    }
    else if (state == "attack") {
        file_path = base_path + "attack.png";
        frame_count = 6;
    }
    else {
        file_path = base_path + "idle.png";
        frame_count = 4;
    }

    SDL_Surface* sheet = IMG_Load(file_path.c_str());
    if (!sheet) {
        std::cerr << "Erreur chargement sprite mage: " << file_path << " - " << IMG_GetError() << std::endl;
        return;
    }
    int frame_width = sheet->w / frame_count;
    int frame_height = sheet->h / 4;  // 4 directions (on n’en utilisera que 2 : gauche/droite)

    int row = (direction == "left") ? 1 : 2;  // 1 = gauche, 2 = droite (comme dans DoctorMonster)

    for (int i = 0; i < frame_count; ++i) {
        SDL_Rect srcRect = { i * frame_width, row * frame_height, frame_width, frame_height };

        SDL_Surface* frameSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, frame_width, frame_height, 32,
                                                         0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

        // Copie du sprite
        SDL_BlitSurface(sheet, &srcRect, frameSurface, nullptr);

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, frameSurface);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        SDL_FreeSurface(frameSurface);
        frames.push_back(texture);
    }


    SDL_FreeSurface(sheet);
    current_frame = 0;
}