#include "Entity.h"
#include "Spear.h"
#include "Bow.h"
#include "Fireball.h"
#include "Fist.h"
#include <iostream>
#include <random>

using namespace std;

int randomRange(int min, int max) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> distrib(min, max);
    return distrib(gen);
}
void Entity::setRandomSize(int minSize, int maxSize){
    // Taille (hitbox)
    this->size = randomRange(minSize, maxSize);
    // On n'utilise plus sprite_scale de manière quadratique ici.
    // On peut l'utiliser comme un multiplicateur visuel si besoin (ex: 1.2 pour être un peu plus grand que la hitbox)
    this->sprite_scale = 1.5f;
}

Entity::Entity(float x, float y, SDL_Renderer* renderer) {
    this->hp = 100;
    this->max_hp = 100;
    this->size = 20;
    this->x = x;
    this->y = y;
    this->current_renderer = renderer;
    sprite_scale = 1.5f;
    loadSprites(renderer);
}
void Entity::loadSprites(SDL_Renderer* renderer) {}
bool Entity::canAttackDistance(Entity* entity) { return this->distance(entity->getX(), entity->getY()) < entity->getWeapon()->getRange(); }
bool Entity::canAttackTime(){ return attack_timer > attack_cooldown; }


double Entity::distance(float x2, float y2){ return sqrt(pow(x2-x, 2)+pow(y2-y, 2)); }
Entity* Entity::findClosestEntity(vector<Entity*> entities){
    if (entities.size() <= 1) return nullptr;
    Entity* closest_entity = (entities[0] == this ? entities[1] : entities[0]);
    double dist = distance(closest_entity->getX(), closest_entity->getY());
    for(Entity* e : entities){
        if(e != this){
            double d = distance(e->getX(), e->getY());
            if(d < dist){
                closest_entity = e;
                dist = d;
            }
        }
    }
    return (closest_entity == this) ? nullptr : closest_entity;
}
void Entity::drawHealthBar(SDL_Renderer* renderer) {
    SDL_Rect border = {int(x-25), int(y-30), 50, 7};
    SDL_Rect health = {int(x-25), int(y-30), 50*hp/max_hp, 7};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &health);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &border);
}

void Entity::setSize(int new_size) { size = (new_size >= 0) ? new_size : 0; }
void Entity::setHp(int new_hp){ hp = (new_hp > 0) ? new_hp : 0; }
void Entity::moveInDirection(float target_x, float target_y){
    setState("run");
    float dx = target_x - x;
    float dy = target_y - y;
    float length = std::sqrt(dx * dx + dy * dy);
    if (length != 0) {
        dx /= length;
        dy /= length;
        x += dx * move_speed * 50.0f / size;
        y += dy * move_speed * 50.0f / size;
    }
    target_x > x ? setDirection("right"): setDirection("left");
}

void Entity::setState(const string& new_state) {
    if (state != new_state) {
        state = new_state;
        current_frame = 0;
        anim_timer = 0;
        loadSprites(current_renderer);
    }
}
void Entity::setDirection(const string& new_dir) {
    if (direction != new_dir) {
        direction = new_dir;
        loadSprites(current_renderer);
    }
}

void Entity::updateAnimation(){
    if (frames.empty()) return;
    anim_timer += 16;
    if (anim_timer >= frame_delay) {
        anim_timer -= frame_delay;
        current_frame = (current_frame + 1) % frames.size();
        if (state == "attack" && current_frame == frames.size() - 1) {
            setState("idle");
        }
    }
}

void Entity::draw(SDL_Renderer* renderer, int time_speed) {
    if (frames.empty()) return;

    // Récupérer la taille réelle de la texture pour garder le ratio
    int texW = 0, texH = 0;
    SDL_QueryTexture(frames[current_frame], nullptr, nullptr, &texW, &texH);

    // Calcul de la taille d'affichage basée sur la taille de la hitbox (size)
    // On applique un facteur (sprite_scale) pour que le sprite soit un peu plus grand que la hitbox
    // si nécessaire (ex: arme qui dépasse).

    // Ratio de l'image
    float ratio = (float)texH / (float)texW;

    // Largeur affichée : proportionnelle à la taille de la hitbox
    int w = int(size * sprite_scale);
    // Hauteur affichée : calculée selon le ratio de l'image pour ne pas déformer
    int h = int(w * ratio);

    SDL_Rect dest = {
        int(x - w / 2),           // Centrer horizontalement sur X
        int(y - h + (size/4)),    // Aligner le bas du sprite (pieds) vers le bas de la hitbox
                                  // (size/4) est un petit ajustement pour que les pieds soient "dans" le rectangle bleu
        w,
        h
    };

    // Dessin du sprite
    SDL_RenderCopy(renderer, frames[current_frame], nullptr, &dest);

    // Dessin de la Hitbox (Rectangle bleu)
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect hitbox = {
        int(x - size / 2),
        int(y - size / 2),
        size,
        size
    };
    SDL_RenderDrawRect(renderer, &hitbox);
}


Guerrier::Guerrier(float x, float y, SDL_Renderer* renderer)
    : Entity(x, y, renderer)
{
    weapon = new Spear(randomRange(35, 45), randomRange(18, 25));
    // baseSpriteSize n'est plus utilisé pour le scale quadratique
    setRandomSize(25, 30);
    this->sprite_scale = 1.8f; // Le sprite est 1.8x plus large que la hitbox (pour l'arme etc)

    foot_offset = 10;
    updateAttackCooldown();
    this->hp = this->max_hp = randomRange(130, 160);
    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f;
    type = "Guerrier";
    loadSprites(renderer);
}

void Guerrier::updateAttackCooldown() {
    this->attack_cooldown = 500 + (weapon->getDamage() * 30);
}

Archer::Archer(float x, float y, SDL_Renderer* renderer)
    : Entity(x, y, renderer)
{
    weapon = new Bow(renderer, randomRange(20, 30), randomRange(180, 220));
    setRandomSize(30, 35);
    this->sprite_scale = 1.5f;

    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f;
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


Mage::Mage(float x, float y, SDL_Renderer* renderer)
    : Entity(x, y, renderer)
{
    weapon = new Fireball(renderer, randomRange(30, 40), randomRange(130, 150));
    // CORRECTION OFFSET MAGE : 100 était beaucoup trop grand
    foot_offset = 10;

    setRandomSize(30, 35);
    this->sprite_scale = 1.6f; // Ajuster selon le visuel

    updateAttackCooldown();
    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f;
    this->hp = this->max_hp = randomRange(50, 70);
    type = "Mage";
    loadSprites(renderer);
}

void Mage::updateAttackCooldown() {
    this->attack_cooldown = 500 + (weapon->getDamage() * 30);
}

Golem::Golem(float x, float y, SDL_Renderer* renderer) : Entity(x, y, renderer)
{
    weapon = new Fist(randomRange(15, 25), randomRange(5, 15));
    foot_offset = 10;
    setRandomSize(45, 50);
    this->sprite_scale = 1.4f; // Golem est costaud

    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f;
    weapon = new Fist(20, 15);
    updateAttackCooldown();
    this->hp = this->max_hp = randomRange(280, 350);
    type = "Tank";
    loadSprites(renderer);
}

void Golem::updateAttackCooldown() {
    this->attack_cooldown = 500 + (weapon->getDamage() * 40);
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

Entity::~Entity() {
    for (auto tex : frames)
        SDL_DestroyTexture(tex);
}