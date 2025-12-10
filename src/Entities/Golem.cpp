#include "Golem.h"
#include "../Weapons/Fist.h"

Golem::Golem(float x, float y, SDL_Renderer* renderer) : Entity(x, y, renderer){
    weapon = new Fist(randomRange(15, 25), randomRange(5, 15));
    foot_offset = 10;
    this->baseSpriteSize = 10.0f;
    setRandomSize(45, 50);
    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f; //+-15%
    weapon = new Fist(20, 15);
    updateAttackCooldown();
    this->hp = this->max_hp = randomRange(280, 350);
    type = "Tank";

    loadSprites(renderer);
}


void Golem::updateAttackCooldown() {
    this->attack_cooldown = 500 + (weapon->getDamage() * 40);
}

void Golem::loadSprites(SDL_Renderer* renderer) {
    // Nettoyage des anciens sprites
    for (auto tex : frames)
        SDL_DestroyTexture(tex);
    frames.clear();

    std::string base_path = "../assets/Golem/";
    std::string file_path;
    int frame_count = 0;

    if (state == "idle") {
        frame_count = 8;
        file_path = base_path + "Golem_1_idle_" + direction + ".png";
    }
    else if (state == "run") {
        frame_count = 10;
        file_path = base_path + "Golem_1_walk_" + direction + ".png";
    }
    else if (state == "attack") {
        frame_count = 11;
        file_path = base_path + "Golem_1_attack_" + direction + ".png";
    }
    else {
        // Par défaut, idle
        frame_count = 8;
        file_path = base_path + "Golem_1_idle_" + direction + ".png";
    }

    SDL_Surface* sheet = IMG_Load(file_path.c_str());
    if (!sheet) {
        cerr << "Erreur chargement sprite Golem: " << file_path
             << " - " << IMG_GetError() << endl;
        return;
    }

    // Chaque frame est sur une ligne
    int frame_width = sheet->w / frame_count;
    int frame_height = sheet->h;

    for (int i = 0; i < frame_count; ++i) {
        SDL_Rect srcRect = { i * frame_width, 0, frame_width, frame_height };

        SDL_Surface* frameSurface = SDL_CreateRGBSurface(SDL_SWSURFACE, frame_width, frame_height, 32,
                                                         0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);
        SDL_BlitSurface(sheet, &srcRect, frameSurface, nullptr);

        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, frameSurface);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

        SDL_FreeSurface(frameSurface);
        frames.push_back(texture);
    }

    SDL_FreeSurface(sheet);
    current_frame = 0;
}