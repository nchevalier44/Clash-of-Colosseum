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

Entity::Entity(int x, int y, SDL_Renderer* renderer) {
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
    return !(SDL_GetTicks() - last_attack_time < attack_cooldown);
}


double Entity::distance(int x2, int y2){
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
    SDL_Rect border = {x-25, y-30, 50, 7};
    SDL_Rect health = {x-25, y-30, 50*hp/max_hp, 7};
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

void Entity::moveInDirection(int x, int y){
    if(x > this->x){
        this->x += 1;
    } else if(x < this->x){
        this->x -= 1;
    }

    if(y > this->y){
        this->y += 1;
    } else if(y < this->y){
        this->y -= 1;
    }
}

void Entity::setState(const string& new_state) {
    if (state != new_state) {
        state = new_state;
        current_frame = 0;
        last_frame_time = SDL_GetTicks();
        loadSprites(current_renderer);
    }
}

void Entity::setDirection(const string& new_dir) {
    if (direction != new_dir) {
        direction = new_dir;
        loadSprites(current_renderer);
    }
}

void Entity::draw(SDL_Renderer* renderer) {
    if (frames.empty()) return;

    Uint32 current_time = SDL_GetTicks();
    if (current_time - last_frame_time > frame_delay) {
        current_frame = (current_frame + 1) % frames.size();
        last_frame_time = current_time;

        if (state == "attack" && current_frame == frames.size() - 1) {
            setState("idle");
        }
    }

    int w = size * sprite_scale;
    int h = size * sprite_scale;
    SDL_Rect dest = { x - w / 2, y - h / 2, w, h };
    SDL_RenderCopy(renderer, frames[current_frame], nullptr, &dest);
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
    SDL_Rect hitbox = { x - size, y - size, size * 2, size * 2 };
    SDL_RenderDrawRect(renderer, &hitbox);
}

Guerrier::Guerrier(int x, int y, SDL_Renderer* renderer)
    : Entity(x, y, renderer)
{
    weapon = new Spear(40, 15);

    // Taille random (hitbox)
    this->size = randomRange(18, 26);

    // Taille du sprite cohérente (un peu plus grande)
    this->sprite_scale = this->size / 20.0f * 2.2f;  // environ 2.0–2.8

    // PV random (entre 85 et 115)
    this->hp = this->max_hp = randomRange(130, 160);

    type = "Guerrier";

    loadSprites(renderer);
}

Archer::Archer(int x, int y, SDL_Renderer* renderer)
    : Entity(x, y, renderer)
{
    weapon = new Bow(25, 200);
    this->size = randomRange(16, 24);
    this->sprite_scale = this->size / 10.0f * 2.0f;
    this->hp = this->max_hp = randomRange(70, 90);
    type = "Archer";
    loadSprites(renderer);
}

Mage::Mage(int x, int y, SDL_Renderer* renderer)
    : Entity(x, y, renderer)
{
    weapon = new Fireball(renderer, 35, 140);
    this->size = randomRange(15, 22);
    this->sprite_scale = this->size/2;
    this->hp = this->max_hp = randomRange(50, 70);
    type = "Mage";
    loadSprites(renderer);
}

Golem::Golem(int x, int y, SDL_Renderer* renderer)
    : Entity(x, y, renderer)
{
    weapon = new Fist(20, 15);
    this->size = randomRange(30, 35);
    this->sprite_scale = this->size / 20.0f * 3.0f;
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
    last_frame_time = SDL_GetTicks();
}

void Guerrier::moveInDirection(int target_x, int target_y) {
    setState("run");

    if (target_x > x) {
        x += 1;
        setDirection("right");
    } else if (target_x < x) {
        x -= 1;
        setDirection("left");
    }

    if (target_y > y) y += 1;
    else if (target_y < y) y -= 1;
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
    last_frame_time = SDL_GetTicks();
}

void Mage::moveInDirection(int target_x, int target_y) {
    setState("run");

    if (target_x > x) {
        x += 1;
        setDirection("right");
    }
    else if (target_x < x) {
        x -= 1;
        setDirection("left");
    }

    if (target_y > y) {
        y += 1;
    }
    else if (target_y < y) {
        y -= 1;
    }
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
    last_frame_time = SDL_GetTicks();
}
void Golem::moveInDirection(int target_x, int target_y) {
    setState("run");

    if (target_x > x) {
        x += 1;
        setDirection("right");
    }
    else if (target_x < x) {
        x -= 1;
        setDirection("left");
    }

    if (target_y > y) {
        y += 1;
    }
    else if (target_y < y) {
        y -= 1;
    }
}
Entity::~Entity() {
    // rien de spécial pour l’instant
    for (auto tex : frames)
        SDL_DestroyTexture(tex);
}