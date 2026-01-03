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
                if (ready_to_attack) {
                    if (this->getWeapon()->type() == "Bow" || this->getWeapon()->type() == "Fireball") {
                        this->getWeapon()->attack(closest, this, all_projectiles, this->getX(), this->getY());
                    } else {
                        this->getWeapon()->attack(closest);
                    }

                    //current cooldown
                    this->resetAttackTimer();
                }
            } else {
                ready_to_attack = false;
            }
        } else {
            this->setState("run");
            this->moveInDirection(closest->getX(), closest->getY());
            ready_to_attack = false;
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

void Entity::setRandomSize(float minSize, float maxSize){
    this->size = base_sprite_size * randomRange(minSize*100, maxSize*100) / 100;
}

Entity::Entity(float x, float y, SDL_Renderer* renderer) {
    this->hp = 100;
    this->max_hp = 100;
    this->x = x;
    this->y = y;
    this->current_renderer = renderer;
    loadSprites(renderer);
}
void Entity::loadSprites(SDL_Renderer* renderer) {
    deleteAllFrames();

    std::vector<std::string> paths = {sprites_file_left, sprites_file_right};
    std::vector<std::string> orientations = {"left", "right"};
    std::vector<std::string> states = {"idle", "run", "attack"};
    for (int i=0; i<2; i++) {
        std::string file_path = paths[i];
        std::string orientation = orientations[i];
        SDL_Surface* sheet = IMG_Load(file_path.c_str());
        if (!sheet) continue;

        int nb_cols = sheet->w / 64; //sprites 64x64 pixels
        for (int row=0; row<3; row++) { // always 3 rows "idle", "run", "attack"
            std::string state = states[row];

            for (int col=0; col<nb_cols; col++) {
                SDL_Rect rect = {col * 64, row * 64, 64, 64};
                SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, 64, 64, 32, sheet->format->format);

                SDL_BlitSurface(sheet, &rect, surface, NULL); //Copy only the right sprite
                if (isFrameEmpty(surface)) {
                    SDL_FreeSurface(surface);
                    continue;
                }

                SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
                if (!texture) continue;
                SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND); // transparency
                frames[state][orientation].push_back(texture);
                SDL_FreeSurface(surface);
            }
        }
        SDL_FreeSurface(sheet);
    }
}

bool Entity::isFrameEmpty(SDL_Surface *surface) {
    if (!surface) return true;

    // 1. Verrouiller la surface pour lire les pixels en sécurité
    if (SDL_LockSurface(surface) != 0) {
        return true; // En cas d'erreur, on considère vide par sécurité
    }

    bool is_empty = true;
    int pixel_count = surface->w * surface->h;

    Uint32* pixels = (Uint32*)surface->pixels;

    // Format des pixels pour extraire les couleurs
    SDL_PixelFormat* format = surface->format;

    for (int i = 0; i < pixel_count; ++i) {
        Uint8 r, g, b, a;
        SDL_GetRGBA(pixels[i], format, &r, &g, &b, &a);

        // Si l'alpha n'est pas 0 (donc le pixel est un peu visible)
        if (a > 0) {
            is_empty = false;
            break; // On a trouvé un pixel, pas besoin de continuer, la frame existe !
        }
    }

    // 2. Déverrouiller la surface
    SDL_UnlockSurface(surface);

    return is_empty;
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
        x += dx * move_speed / size;
        y += dy * move_speed / size;
    }

    // Mise à jour de la direction pour les sprites
    target_x > x ? setDirection("right"): setDirection("left");
}

void Entity::setState(const string& new_state) {
    if (state != new_state) {
        state = new_state;
        current_frame = 0;
        anim_timer = 0;
        ready_to_attack = false;
    }
}

void Entity::setDirection(const string& new_dir) {
    if (direction != new_dir) {
        direction = new_dir;
    }
}

void Entity::updateAnimation(){
    if (frames[state][direction].empty()) return;

    anim_timer += 16; // On simule qu'une frame de jeu (16ms) vient de passer

    if (anim_timer >= frame_delay) {
        anim_timer -= frame_delay;
        current_frame = (current_frame + 1) % frames[state][direction].size();

        if (state == "attack") {
            if (current_frame == frame_to_attack) {
                ready_to_attack = true; //He can attack according to the animation
            }
            if (current_frame == frames[state][direction].size() - 1) {
                setState("idle"); // We set the entity to idle because he has to wait before another attack (cooldown)
            }

        }
    }
}

void Entity::draw(SDL_Renderer* renderer) {
    if (frames[state][direction].empty()) return;

    int s = size * 64; // Taille d'affichage du sprite
    int w = s * ratio_hitbox_width;   // largeur de la hitbox
    int h = s * ratio_hitbox_height;   // hauteur de la hitbox

    SDL_Rect dest = {
        int(x - s/2),   // centrer horizontalement
        int(y - s + h/2), // milieu de l'entité
        s,
        s
    };

    SDL_RenderCopy(renderer, frames[state][direction][current_frame], nullptr, &dest);

    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

    //Display hitbox
    /*SDL_Rect hitbox = {
        int(x - w/2),
        int(y - h/2),
        w,
        h
    };
    SDL_RenderDrawRect(renderer, &hitbox);*/
}

Entity::~Entity() {
    stopThread();
    deleteAllFrames();
}

void Entity::deleteAllFrames() {
    for (auto& state_pair : frames) {
        for (auto& direction_pair : state_pair.second) {
            for (SDL_Texture* texture : direction_pair.second) {
                if (texture) {
                    SDL_DestroyTexture(texture);
                }
            }
        }
    }
    frames.clear();
}