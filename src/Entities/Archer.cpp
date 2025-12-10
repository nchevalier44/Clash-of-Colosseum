#include "Archer.h"
#include "../Weapons/Bow.h"

Archer::Archer(float x, float y, SDL_Renderer* renderer) : Entity(x, y, renderer){
    weapon = new Bow(renderer, randomRange(20, 30), randomRange(180, 220));

    this->baseSpriteSize = 10.0f;
    setRandomSize(30, 35);
    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f; //+-15%
    this->hp = this->max_hp = randomRange(70, 90);
    type = "Archer";

    loadSprites(renderer);
}

void Archer::updateAttackCooldown() {
    this->attack_cooldown = 500 + (weapon->getDamage() * 30);
}

void Archer::loadSprites(SDL_Renderer* renderer) {
    // Nettoyage
    for (auto tex : frames) SDL_DestroyTexture(tex);
    frames.clear();

    std::string file_path = "../assets/Archer/archer_" + direction + ".png";
    SDL_Surface* sheet = IMG_Load(file_path.c_str());
    if (!sheet) {
        // Fallback si le fichier left n'existe pas encore
        sheet = IMG_Load("../assets/Archer/archer_right.png");
        if (!sheet) return;
    }

    int frame_count = 0;
    int row_index = 0;

    // Configuration
    if (state == "idle") {
        row_index = 0;
        frame_count = 5;
    }
    else if (state == "attack") {
        row_index = 1;
        // Si ça clignote, c'est que tu n'as pas vraiment 11 frames pleines.
        // Essayons 10 frames pour voir si ça règle le souci.
        frame_count = 11;
    }
    else if (state == "run") {
        row_index = 2;
        frame_count = 8;
    }
    else if (state == "death") {
        row_index = 4;
        frame_count = 6;
    }
    else {
        row_index = 0;
        frame_count = 5;
    }

    // --- CALCUL MAGIQUE DU DÉCALAGE ---
    int max_cols = 11; // La largeur de ta grille (basée sur la ligne Attack)
    int start_column = 0;

    // Si on regarde à gauche, les cases vides sont au DÉBUT de la ligne.
    // On doit donc sauter ces cases vides.
    if (direction == "left") {
        // Ex: Pour Run (8 frames) sur 11 colonnes : 11 - 8 = 3 cases vides au début
        start_column = max_cols - frame_count;
    }

    int total_rows = 5;
    int frame_width = sheet->w / max_cols;
    int frame_height = sheet->h / total_rows;

    for (int i = 0; i < frame_count; ++i) {
        // On applique le décalage calculé
        int current_col = i + start_column;

        SDL_Rect srcRect = {
                current_col * frame_width,
                row_index * frame_height,
                frame_width,
                frame_height
        };

        SDL_Surface* frameSurface = SDL_CreateRGBSurface(
                SDL_SWSURFACE, frame_width, frame_height, 32,
                0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000
        );

        SDL_BlitSurface(sheet, &srcRect, frameSurface, nullptr);
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, frameSurface);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        SDL_FreeSurface(frameSurface);
        frames.push_back(texture);
    }

    SDL_FreeSurface(sheet);
}
