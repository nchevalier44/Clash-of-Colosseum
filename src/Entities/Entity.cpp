#include "Entity.h"
#include "../Weapons/Bow.h"

void Entity::startThread(std::vector<Entity*>* all_entities, std::vector<Projectile*>* all_projectiles, int* game_time_speed, bool* same_type_peace, std::mutex* global_mutex) {
    this->thread_is_running = true;
    this->entity_thread = std::thread(&Entity::threadLoop, this, all_entities, all_projectiles, game_time_speed, same_type_peace, global_mutex);
}

void Entity::stopThread() {
    if(!thread_is_running) return;

    this->thread_is_running = false;
    if(this->entity_thread.joinable()){
        this->entity_thread.join();
    }
}

void Entity::threadUpdate(std::vector<Entity*>* all_entities, std::vector<Projectile*>* all_projectiles, bool* same_type_peace,  std::mutex* global_mutex){
    //On verrouille l'accès aux données partagées (le vecteur all_entities)
    std::lock_guard<std::mutex> lock(*global_mutex);

    Entity* closest = findClosestEntity(*all_entities, *same_type_peace);

    if(closest){
        if(this->canAttackDistance(closest)){
            //On force l'entité à regarder sa cible (orientation automatique)
            if (closest->getX() < this->getX()) {
                this->setDirection("left");
            } else {
                this->setDirection("right");
            }

            if (this->canAttackTime()) {
                this->setState("attack");
                if (this->getWeapon()->type() == "Bow" || this->getWeapon()->type() == "Fireball") {
                    this->getWeapon()->attack(closest, this, all_projectiles, this->getX(), this->getY());
                } else {
                    this->getWeapon()->attack(closest);
                }

                this->resetAttackTimer();

            }
        } else {
            this->setState("run");
            this->moveInDirection(closest->getX(), closest->getY());
        }
    }
    this->addAttackTimer();
}

void Entity::threadLoop(std::vector<Entity*>* all_entities, std::vector<Projectile*>* all_projectiles, int* game_time_speed, bool* same_type_peace, std::mutex* global_mutex) {
    while(thread_is_running && hp > 0){

        for(int i = 0; i < *game_time_speed; i++){
            if(hp <= 0) break; //On arrête si on est mort
            this->threadUpdate(all_entities, all_projectiles, same_type_peace, global_mutex);
        }

        //Pour 60 fps = 16ms de pause
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
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
    return this->distance(entity->getX(), entity->getY()) < this->getWeapon()->getRange();
}

bool Entity::canAttackTime(){
    return attack_timer > attack_cooldown;
}


double Entity::distance(float x2, float y2){
    return sqrt(pow(x2-x, 2)+pow(y2-y, 2));
}

Entity* Entity::findClosestEntity(vector<Entity*> entities, bool ignoreSameType){
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

Entity::~Entity() {
    stopThread();
    for (auto tex : frames)
        SDL_DestroyTexture(tex);
}