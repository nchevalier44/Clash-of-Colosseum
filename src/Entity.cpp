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

    // Échelle du sprite (pour qu'il corresponde visuellement à la hitbox)
    this->sprite_scale = this->size / this->baseSpriteSize;
}

Entity::Entity(float x, float y, SDL_Renderer* renderer) {
    this->hp = 100;
    this->max_hp = 100;
    this->size = 20;
    this->x = x;
    this->y = y;
    this->current_renderer = renderer;
    sprite_scale = 1;
    loadSprites(renderer);
}

void Entity::loadSprites(SDL_Renderer* renderer) {
    // par défaut : rien, redéfini dans les classes enfants
}

bool Entity::canAttackDistance(Entity* entity) {
    return this->distance(entity->getX(), entity->getY()) < entity->getWeapon()->getRange();
}

bool Entity::canAttackTime(){
    return attack_timer > attack_cooldown;
}

double Entity::distance(float x2, float y2){
    return sqrt(pow(x2-x, 2)+pow(y2-y, 2));
}

Entity*Entity::findClosestEntity(vector<Entity*> entities, bool ignoreSameType){
    if (entities.empty()) return nullptr;

    Entity* closest_entity = nullptr;
    double min_dist = 100000000.0; // Valeur très grande

    for(Entity* e : entities){
        // On ne se cible pas soi-même
        if(e == this) continue;

        // Si l'option est activée et que c'est le même type, on ignore
        if(ignoreSameType && e->getType() == this->getType()) continue;

        double d = distance(e->getX(), e->getY());
        if(d < min_dist){
            closest_entity = e;
            min_dist = d;
        }
    }

    return closest_entity;
}

void Entity::drawHealthBar(SDL_Renderer* renderer) {
    SDL_Rect border = {int(x-25), int(y-30), 50, 7};
    SDL_Rect health = {int(x-25), int(y-30), 50*hp/max_hp, 7};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &health);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &border);
}

void Entity::setSize(int new_size) {
    if (new_size >= 0) {
        size = new_size;
    } else {
        size = 0;
    }
}

void Entity::setHp(int new_hp){
    if(new_hp > 0){
        hp = new_hp;
    } else{
        hp = 0;
    }
}

void Entity::moveInDirection(float target_x, float target_y){
    setState("run");
    float dx = target_x - x;
    float dy = target_y - y;
    float length = std::sqrt(dx * dx + dy * dy);

    if (length != 0) {
        // Normalisation (pour ne pas aller plus vite en diagonale)
        dx /= length;
        dy /= length;

        // Application de la vitesse en fonction de la taille
        x += dx * move_speed * 50.0f / size;
        y += dy * move_speed * 50.0f / size;
    }

    // Mise à jour de la direction pour les sprites
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

    anim_timer += 16; // On simule qu'une frame de jeu (16ms) vient de passer

    Uint32 current_time = SDL_GetTicks();
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

    int w = size * sprite_scale; // largeur du sprite
    int h = size * sprite_scale; // hauteur du sprite

    SDL_Rect dest = {
        int(x - w / 2),           // centrer horizontalement
        int(y - h + foot_offset), // sprite au-dessus de la hitbox
        w,
        h
    };

    SDL_RenderCopy(renderer, frames[current_frame], nullptr, &dest);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    // --- MODIFICATION ICI ---
    // Calcul du centre vertical du sprite pour y placer la hitbox
    // Centre Y du sprite = (y - h + foot_offset) + h/2
    // Position Y hitbox = Centre Y du sprite - size/2

    SDL_Rect hitbox = {
        int(x - size / 2),
        int(y - h / 2 + foot_offset - size / 2), // Hitbox centrée sur le visuel
        size,
        size
    };
    SDL_RenderDrawRect(renderer, &hitbox);
}
void Golem::draw(SDL_Renderer* renderer, int time_speed) {
    if (frames.empty()) return;
    int w = size * sprite_scale; // largeur du sprite
    int h = size * sprite_scale; // hauteur du sprite

    SDL_Rect dest = {
        int(x - w / 2),           // centrer horizontalement
        int(y - h + foot_offset), // sprite au-dessus de la hitbox
        w,
        h
    };

    SDL_RenderCopy(renderer, frames[current_frame], nullptr, &dest);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    // --- MODIFICATION ICI ---
    // Calcul du centre vertical du sprite pour y placer la hitbox
    // Centre Y du sprite = (y - h + foot_offset) + h/2
    // Position Y hitbox = Centre Y du sprite - size/2

    SDL_Rect hitbox = {
        int(x - size / 2),
        int(y - h / 2.3), // Hitbox centrée sur le visuel
        size,
        size
    };
    SDL_RenderDrawRect(renderer, &hitbox);
}
void Guerrier::draw(SDL_Renderer* renderer, int time_speed) {
    if (frames.empty()) return;
    int w = size * sprite_scale; // largeur du sprite
    int h = size * sprite_scale; // hauteur du sprite

    SDL_Rect dest = {
        int(x - w / 2),
        int(y - h / 2),
        w,
        h
    };

    SDL_RenderCopy(renderer, frames[current_frame], nullptr, &dest);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    // --- MODIFICATION ICI ---
    // Calcul du centre vertical du sprite pour y placer la hitbox
    // Centre Y du sprite = (y - h + foot_offset) + h/2
    // Position Y hitbox = Centre Y du sprite - size/2

    SDL_Rect hitbox = {
        int(x - size / 2),
        int(y - h / 6), // Hitbox centrée sur le visuel
        size,
        size
    };
    SDL_RenderDrawRect(renderer, &hitbox);
}
void Mage::draw(SDL_Renderer* renderer, int time_speed) {
    if (frames.empty()) return;
    int w = size * sprite_scale; // largeur du sprite
    int h = size * sprite_scale; // hauteur du sprite

    SDL_Rect dest = {
        int(x - w / 2),
        int(y - h / 2),
        w,
        h
    };

    SDL_RenderCopy(renderer, frames[current_frame], nullptr, &dest);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    // --- MODIFICATION ICI ---
    // Calcul du centre vertical du sprite pour y placer la hitbox
    // Centre Y du sprite = (y - h + foot_offset) + h/2
    // Position Y hitbox = Centre Y du sprite - size/2

    SDL_Rect hitbox = {
        int(x - size / 2.3),
        int(y - h / 10), // Hitbox centrée sur le visuel
        size,
        size
    };
    SDL_RenderDrawRect(renderer, &hitbox);
}

void Archer::draw(SDL_Renderer* renderer, int time_speed) {
    if (frames.empty()) return;
    int w = size * sprite_scale; // largeur du sprite
    int h = size * sprite_scale; // hauteur du sprite

    SDL_Rect dest = {
        int(x - w / 2),
        int(y - h / 2),
        w,
        h
    };

    SDL_RenderCopy(renderer, frames[current_frame], nullptr, &dest);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    // --- MODIFICATION ICI ---
    // Calcul du centre vertical du sprite pour y placer la hitbox
    // Centre Y du sprite = (y - h + foot_offset) + h/2
    // Position Y hitbox = Centre Y du sprite - size/2

    SDL_Rect hitbox = {
        int(x - size / 2),
        int(y - h / 6), // Hitbox centrée sur le visuel
        size,
        size
    };
    SDL_RenderDrawRect(renderer, &hitbox);
}


Guerrier::Guerrier(float x, float y, SDL_Renderer* renderer)
    : Entity(x, y, renderer)
{
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

Archer::Archer(float x, float y, SDL_Renderer* renderer)
    : Entity(x, y, renderer)
{
    weapon = new Bow(renderer, randomRange(20, 30), randomRange(180, 220));

    this->baseSpriteSize = 25.0f;
    setRandomSize(50, 55);
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


Mage::Mage(float x, float y, SDL_Renderer* renderer)
    : Entity(x, y, renderer)
{
    weapon = new Fireball(renderer, randomRange(30, 40), randomRange(130, 150)); foot_offset = 100;
    this->baseSpriteSize = 5.0f;
    setRandomSize(30, 35);
    updateAttackCooldown();
    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f; //+-15%
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
    this->baseSpriteSize = 40.0f;
    setRandomSize(80, 90);
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
    int frame_height = sheet->h / 4; // 4 directions (on n’en utilisera que 2 : gauche/droite)

    int row = (direction == "left") ? 1 : 2; // 1 = gauche, 2 = droite (comme dans DoctorMonster)

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