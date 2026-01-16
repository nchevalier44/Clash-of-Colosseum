#include "Entity.h"
#include "../Weapons/Bow.h"
#include "../Graphics.h"

void Entity::startThread(std::vector<Entity*>* all_entities, std::vector<Projectile*>* all_projectiles, bool* same_type_peace, std::mutex* global_mutex) {
    if (thread_is_running) return;
    this->thread_is_running = true;
    // Lance "l'IA" de l'entité dans un thread séparé pour qu'elle "réfléchisse" en parallèle du rendu graphique
    this->entity_thread = std::thread(&Entity::threadLoop, this, all_entities, all_projectiles, same_type_peace, global_mutex);
}

void Entity::stopThread() {
    if(!thread_is_running) return;

    this->thread_is_running = false;
    // Attend proprement la fin du thread avant de continuer (évite les crashs à la destruction)
    if(this->entity_thread.joinable()){
        this->entity_thread.join();
    }
}

// C'est le "cerveau" de l'entité, exécuté à chaque itération de la boucle de jeu
void Entity::threadUpdate(std::vector<Entity*>* all_entities, std::vector<Projectile*>* all_projectiles, bool* same_type_peace,  std::mutex* global_mutex){
    bool should_find_entity = false;

    if (find_entity_timer <= 0) {
        should_find_entity = true;
        // Optimisation : On étale la recherche de cible sur plusieurs frames avec un délai aléatoire
        // pour pas que toutes les entités utilisent findClosestEntity en meme temps et tout le temps
        find_entity_timer = find_entity_delay + (std::rand() % 5);
    } else {
        find_entity_timer--;
    }

    if (should_find_entity) {
        // On verrouille l'accès aux données partagées (liste des entités)
        // pour éviter qu'une entité ne soit supprimée pendant qu'on la lit.
        std::lock_guard<std::mutex> lock(*global_mutex);

        Entity* closest = findClosestEntity(*all_entities, *same_type_peace);

        if(closest && closest->getHp() > 0){
            has_active_target = true;
            last_target_x = closest->getX();
            last_target_y = closest->getY();

            // Logique de combat
            if(this->canAttackDistance(closest)){
                // Orientation vers la cible
                if (last_target_x < this->getX()) this->setDirection("left");
                else this->setDirection("right");

                if (this->canAttackTime()) {
                    this->setState("attack");

                    // 'ready_to_attack' est mis à true par le système d'animation (voir updateAnimation plus bas)
                    // quand le sprite atteint la frame précise de l'impact / du tir (ex: quand l'arc est tendu).
                    // C'est pour éviter des dégâts décalés par rapport à l'animation
                    if (ready_to_attack) {
                        if (this->getWeapon()->type() == "Bow" || this->getWeapon()->type() == "Fireball") {
                            this->getWeapon()->attack(closest, this, all_projectiles, this->getX(), this->getY());
                        } else {
                            this->getWeapon()->attack(closest);
                        }
                        ready_to_attack = false;
                        is_attacking = true;
                        this->resetAttackTimer();
                    }
                }
            } else {
                // Trop loin pour attaquer
                ready_to_attack = false;
            }
        } else {
            // Pas de cible
            has_active_target = false;
            ready_to_attack = false;
        }
    }

    // Optimisation : Cette phase se fait locker le mutex.
    // On utilise les dernières coordonnées connues (last_target_x/y) pour bouger.
    // Cela permet aux threads de tourner en parallèle sans s'attendre les uns les autres.
    if (has_active_target && !is_attacking) {
        float dx = last_target_x - x;
        float dy = last_target_y - y;
        float distSq = dx*dx + dy*dy;
        float range = this->getWeapon()->getRange();

        // Si on est trop loin pour attaquer, on court vers la cible
        if (distSq > range * range) {
            this->setState("run");
            this->moveInDirection(last_target_x, last_target_y);
        }
    }

    this->addAttackTimer();
}

void Entity::threadLoop(std::vector<Entity*>* all_entities, std::vector<Projectile*>* all_projectiles, bool* same_type_peace, std::mutex* global_mutex) {
    while(thread_is_running && hp > 0){

        if (!pause) {
            // Gestion de l'accélération du temps (x1, x2, x5...)
            // Si la vitesse est x5, on exécute la logique 5 fois
            for(int i = 0; i < Graphics::game_time_speed; i++){
                if(hp <= 0) break;
                this->threadUpdate(all_entities, all_projectiles, same_type_peace, global_mutex);
            }
        }

        // Pause de 16ms pour simuler ~60 mises à jour par seconde et ne pas surcharger le CPU.
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

// Découpe la feuille de sprites (spritesheet) en textures individuelles
void Entity::loadSprites(SDL_Renderer* renderer) {
    deleteAllFrames();

    std::vector<std::string> paths = {sprites_file_left, sprites_file_right};
    std::vector<std::string> orientations = {"left", "right"};
    // L'ordre est important : ligne 0 = idle, ligne 1 = run, ligne 2 = attack
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

    // Verrouille la surface pour lire les pixels en sécurité
    if (SDL_LockSurface(surface) != 0) {
        return true;
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

    // Valeur très grande de base pour que la première entité soit la plus proche au début
    double min_dist = 100000000.0;

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

void Entity::setSize(float new_size) {
    if (new_size >= 0.5) {
        size = new_size;
    } else {
        size = 0.5;
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
    float current_x = x;
    float current_y = y;
    //x et y atomic donc on les charge pour les "prendre" qu'une fois

    float dx = target_x - current_x;
    float dy = target_y - current_y;
    float length = std::sqrt(dx * dx + dy * dy);

    if (length != 0) {
        // Normalisation (pour ne pas aller plus vite en diagonale)
        dx /= length;
        dy /= length;

        // Les entités plus grosses sont plus lentes (divisé par size)
        x += dx * move_speed / size;
        y += dy * move_speed / size;
    }

    // Mise à jour de la direction pour les sprites
    target_x > current_x ? setDirection("right"): setDirection("left");
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

    // On simule qu'une frame de jeu (16ms) multiplié par la vitesse de jeu vient de passer
    anim_timer += 16 * Graphics::game_time_speed;
    if (anim_timer < frame_delay) return;

    // Calcul du nombre de frames à passer (si le jeu va très vite)
    int frames_to_skip = anim_timer / frame_delay;
    anim_timer = int(anim_timer) % frame_delay;

    int total_frames = frames[state][direction].size();
    int next_theoritical_frame = current_frame + frames_to_skip;

    if (state == "attack"){

        // Si on atteint la "frame d'attaque", on signale à l'entité qu'elle a le droit d'attaquer
        if (current_frame < frame_to_attack && next_theoritical_frame >= frame_to_attack) {
            ready_to_attack = true;
            current_frame = frame_to_attack;
            return;
        }

        if (next_theoritical_frame >= total_frames) {
            is_attacking = false;
            // We set the entity to idle because he has to wait before another attack (cooldown)
            setState("idle");
            return;
        }
        current_frame = next_theoritical_frame;
    } else {
        current_frame = (current_frame + frames_to_skip) % total_frames;
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