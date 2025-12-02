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
    std::cout << "Size : " << this->size << " | BaseSpriteSize : " << this->baseSpriteSize << std::endl;
    this->sprite_scale = this->size / this->baseSpriteSize;
    std::cout << "Sprite scale : " << this->sprite_scale << std::endl;
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

Entity* Entity::findClosestEntity(vector<Entity*> entities){
    if (entities.size() <= 1) {
        return nullptr;
    }

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

    if(closest_entity == this){
        return nullptr;
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
    }  else{
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

    int w = size * sprite_scale;   // largeur du sprite
    int h = size * sprite_scale;   // hauteur du sprite

    SDL_Rect dest = {
        int(x - w / 2),   // centrer horizontalement
        int(y - h + foot_offset),       // sprite au-dessus de la hitbox
        w,
        h
    };

    SDL_RenderCopy(renderer, frames[current_frame], nullptr, &dest);

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
    weapon = new Spear(randomRange(35, 45), randomRange(18, 25)); //dégat, range

    this->baseSpriteSize = 9.0f;
    setRandomSize(25, 30);

    foot_offset = 10;
    this->hp = this->max_hp = randomRange(130, 160);
    this->attack_cooldown = 500 + (weapon->getDamage() * 30);
    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f; //+-15%
    type = "Guerrier";
    loadSprites(renderer);
}

Archer::Archer(float x, float y, SDL_Renderer* renderer)
    : Entity(x, y, renderer)
{
    weapon = new Bow(randomRange(20, 30), randomRange(180, 220));

    this->baseSpriteSize = 10.0f;
    setRandomSize(30, 35);
    this->attack_cooldown = 500 + (weapon->getDamage() * 30);
    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f; //+-15%
    this->hp = this->max_hp = randomRange(70, 90);
    type = "Archer";

    loadSprites(renderer);
}
void Archer::loadSprites(SDL_Renderer* renderer) {
    // Nettoyer les anciens sprites
    for (auto tex : frames)
        SDL_DestroyTexture(tex);
    frames.clear();

    std::string file_path = "../assets/Archer/archer.png";

    SDL_Surface* sheet = IMG_Load(file_path.c_str());
    if (!sheet) {
        std::cerr << "Erreur chargement sprite archer: "
                  << file_path << " - " << IMG_GetError() << std::endl;
        return;
    }

    // Définir le nombre de frames selon l'état
    int frame_count = 0;
    int row = 0;

    if (state == "idle") {
        frame_count = 5;
        row = (direction == "left") ? 0 : 1;
    }
    else if (state == "run") {
        frame_count = 8;
        row = (direction == "left") ? 2 : 3;
    }
    else if (state == "attack") {
        frame_count = 5;
        row = (direction == "left") ? 4 : 5;
    }
    else if (state == "death") {
        frame_count = 6;
        row = (direction == "left") ? 6 : 7;
    }
    else {
        // Sécurité → défaut = idle
        frame_count = 5;
        row = (direction == "left") ? 0 : 1;
    }

    int total_rows = 8; // ton spritesheet a 8 lignes

    // Taille d'une frame
    int frame_width = sheet->w / frame_count;
    int frame_height = sheet->h / total_rows;

    for (int i = 0; i < frame_count; ++i) {
        SDL_Rect srcRect = { i * frame_width, row * frame_height, frame_width, frame_height };

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

    current_frame = 0;
}


Mage::Mage(float x, float y, SDL_Renderer* renderer)
    : Entity(x, y, renderer)
{
    weapon = new Fireball(renderer, randomRange(30, 40), randomRange(130, 150));    foot_offset = 100;
    this->baseSpriteSize = 5.0f;
    setRandomSize(30, 35);

    this->attack_cooldown = 500 + (weapon->getDamage() * 30);
    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f; //+-15%
    this->hp = this->max_hp = randomRange(50, 70);
    type = "Mage";

    loadSprites(renderer);
}

Golem::Golem(float x, float y, SDL_Renderer* renderer)
    : Entity(x, y, renderer)
{
    weapon = new Fist(randomRange(15, 25), randomRange(5, 15));
    foot_offset = 10;
    this->baseSpriteSize = 10.0f;
    setRandomSize(45, 50);
    this->attack_cooldown = 500 + (weapon->getDamage() * 40);
    this->move_speed *= 1.0f + randomRange(-15, 15) / 100.f; //+-15%
    this->hp = this->max_hp = randomRange(280, 350);
    type = "Tank";

    loadSprites(renderer);
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