#include "Graphics.h"
#include "Weapons/Bow.h"
#include "Menu/GameMenu.h"
#include "Entities/Guerrier.h"
#include "Entities/Archer.h"
#include "Entities/Mage.h"
#include "Entities/Golem.h"
#include <iostream>

std::atomic<int> Graphics::game_time_speed(1);
std::chrono::high_resolution_clock::time_point Graphics::last_change_speed(std::chrono::high_resolution_clock::now());
SimulationStats* Graphics::simulations_stats(nullptr);

void Graphics::changeGameSpeed() {
    // Pour que le graphique de statistiques durée soient bons
    // A chaque changement de vitesse, on calcul la durée que l'utilisateur est restée à cette vitesse
    // Puis on la multiplie par la vitesse (2, 20, 50, ...) pour avoir la durée réel qu'il aurait mis
    // s'il était resté en x1 pour obtenir ce résultat

    auto now = std::chrono::high_resolution_clock::now();

    if (simulations_stats) {
        auto elapsed_real_time = now - last_change_speed;
        auto duration = elapsed_real_time * game_time_speed.load(); //load car c'est un atomic
        std::chrono::duration<float, std::milli> duration_from_start = simulations_stats->getDuration();
        simulations_stats->setDuration(duration_from_start + duration);
    }
    last_change_speed = now;
}

Graphics::Graphics(SDL_Window* window, SDL_Renderer* renderer) : window(window), renderer(renderer) {
    gameMusic = Mix_LoadMUS("../assets/gamemusic.mp3");
    if (!gameMusic) {
        std::cerr << "Erreur chargement musique jeu: " << Mix_GetError() << std::endl;
    }
    backgroundTexture = IMG_LoadTexture(renderer, "../assets/background.png");
    if (!backgroundTexture) {
        std::cerr << "Erreur chargement background: " << IMG_GetError() << std::endl;
    }
    game_menu = new GameMenu(renderer, window);
}

SDL_Renderer* Graphics::getRenderer() const {
    return renderer;
}

Graphics::~Graphics() {
    if (!entities.empty()) {
        entities.clear();
    }
    if (gameMusic) {
        Mix_HaltMusic();
        Mix_FreeMusic(gameMusic);
        gameMusic = nullptr;
    }

}

void Graphics::setEntities(const std::vector<Entity*>& ents) {
    entities = ents;
}

void Graphics::startAllEntitiesThread(){
    for(Entity* e : entities){
        e->startThread(&entities, &projectiles, &same_type_peace, &global_mutex);
    }
}

void Graphics::stopAllEntitiesThread(){
    for(Entity* e : entities){
        e->stopThread();
    }
}

void Graphics::pauseAllEntities(bool pause) {
    for(Entity* e : entities) {
        e->setPause(pause);
    }
}

void Graphics::updateEntities(bool draw){

    for (Entity* e : entities) {
        if(e->getHp() <= 0){
            entities_to_delete.push_back(e);
            continue;
        }

        e->updateAnimation();

        if (!draw) return;

        e->draw(renderer);
        if(showHealthBars) e->drawHealthBar(renderer);

        if (game_menu->getSelectedEntity() == e) {
            int s = e->getSize() * 64; // Taille d'affichage du sprite
            int w = s * e->getRatioHitboxWidth();   // largeur de la hitbox
            int h = s * e->getRatioHitboxHeight();   // hauteur de la hitbox

            SDL_Rect hitbox = {
                int(e->getX() - w/2),
                int(e->getY() - h/2),
                w,
                h
            };
            SDL_SetRenderDrawColor(renderer, 230, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &hitbox);
        }
    }
}

void Graphics::updateProjectiles(bool draw){

    // On copie les pointeurs pour pouvoir travailler dessus sans bloquer les autres
    global_mutex.lock();
    std::vector<Projectile*> projectiles_snapshot;
    global_mutex.unlock();
    // ICI LE VERROU EST RELACHÉ ! Les entités peuvent jouer !

    // --- ETAPE 2 : CALCULS PHYSIQUES (Déverrouillé) ---
    // On note qui est mort dans une liste locale pour ne pas modifier le vecteur global sans lock
    std::vector<Projectile*> dead_projectiles;

    for (Projectile* proj : projectiles_snapshot) {

        // Si le projectile a déjà été supprimé ailleurs (rare mais possible), on saute
        if (!proj) continue;

        // Fin de vie (Temps)
        if(SDL_GetTicks() >= proj->getEndTime()){
            dead_projectiles.push_back(proj);
            continue;
        }

        proj->move(); // Calcul de position (Safe si x/y projectile ne sont touchés que par ici)

        if(draw) proj->draw(renderer);

        // Collisions
        // Optimisation : variables locales pour éviter les accès mémoire lents
        float projX = proj->getX();
        float projY = proj->getY();
        float projDmg = proj->getDamage();
        Entity* owner = proj->getOwner();
        float ownerSize = (owner) ? owner->getSize() : 1.0f;
        float damage = projDmg * 1.3f * ownerSize;

        // On parcourt les entités.
        // Comme entities n'est modifié (ajout/suppression) que par le MainThread,
        // et qu'on EST le MainThread, on peut lire le vecteur sans lock.
        // La lecture des positions des entités (x, y) est safe car elles sont std::atomic.
        bool hit = false;
        for (Entity* e : entities) {
            if(owner != nullptr && e != owner && e->getHp() > 0){
                float dx = e->getX() - projX;
                float dy = e->getY() - projY;
                float sumRadius = e->getSize() + 10;

                // Distance au carré pour éviter sqrt (très coûteux)
                if ((dx*dx + dy*dy) <= (sumRadius * sumRadius)) {
                    // setHp est thread-safe (atomic)
                    e->setHp(e->getHp() - (int)damage);
                    hit = true;
                    break;
                }
            }
        }

        if(hit){
            dead_projectiles.push_back(proj);
        }
    }

    // --- ETAPE 3 : NETTOYAGE (Verrouillé) ---
    // On reprend le verrou pour supprimer réellement les morts du vecteur global
    if (!dead_projectiles.empty()) {
        std::lock_guard<std::mutex> lock(global_mutex);

        for (Projectile* dead : dead_projectiles) {
            // On cherche et supprime le projectile du vrai vecteur
            auto it = std::find(projectiles.begin(), projectiles.end(), dead);
            if (it != projectiles.end()) {
                delete *it; // On libère la mémoire
                projectiles.erase(it); // On réduit le vecteur
            }
        }
    }
}

void Graphics::update(bool* running, bool* keep_playing) {

    handleEvent(running, keep_playing);

    //Exit button have been clicked so we stop the simulation
    if (game_menu->isSimulationStopped()) {
        stopAllEntitiesThread();
        deleteAllProjectiles();
        *running = false;
        return;
    }

    //Game is not running so we do nothing (we wait handleEvent)
    if (is_game_paused || end_of_game) return;

    // On dessine l'image de fond. Le NULL, NULL signifie "toute l'image" sur "toute la fenêtre"
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    } else {
        // Si l'image n'a pas chargé, on met une couleur uni
        SDL_SetRenderDrawColor(renderer, 230, 198, 34, 255);
        SDL_RenderClear(renderer);
    }

    for(int i = 0; i<game_time_speed; i++) { // On répète l'action plusieurs fois si le temps est plus rapide

        bool draw = i == game_time_speed-1; //On dessine que si c'est la dernière update de la boucle (
        updateEntities(draw);
        updateProjectiles(draw);

        if (!entities_to_delete.empty()) {
            deleteDeadEntities();

            //Il ne reste qu'un seul type d'entité, ils ne s'attaquent pas (same_type_peace), donc on arrête la simulation
            if (same_type_peace && !multipleTypesAreAlive()) {
                end_of_game = true;
                stopAllEntitiesThread();
                std::map<std::string, int> dico = getNumberEntitiesPerTypes();
                for (auto pair : dico) {
                    if (pair.second != 0) game_menu->displayEndSimulation(pair, generation);
                    break;
                }
                return;
            }
        }


        //Reproduction
        if (entities.size() <= min_number_entity){
            std::lock_guard<std::mutex> lock(global_mutex);
            pauseAllEntities(true);
            increaseAllEntitiesAge();
            deleteAllProjectiles();
            generation++;
            simulations_stats->updateLastGeneration(entities.size());
            std::vector<Entity*> new_entities; //Temporary vector
            std::vector<Entity*> parents_left = entities;

            while (parents_left.size() >= 2) {
                int number_of_children = std::rand() % 5 + 1;

                Entity* p1 = parents_left[0];
                int i = 1;
                //Les parents doivent avoir le même type si same_type_peace est activé
                if (same_type_peace) {
                    while (i < parents_left.size() && parents_left[i]->getType() != p1->getType()) i++;
                    if (i == parents_left.size()) { // Aucun parents ne correspond
                        parents_left.erase(parents_left.begin());
                        continue;
                    }
                }
                Entity* p2 = parents_left[i];

                for(int j = 0; j<number_of_children; j++){
                    Entity* new_entity = this->createNewEntityFromParents(p1, p2);
                    new_entities.push_back(new_entity);
                }
                //On supprime les deux parents de la liste de ceux qui reste
                parents_left.erase(parents_left.begin() + i); // p2
                parents_left.erase(parents_left.begin()); // p1
            }
            entities.insert(entities.end(), new_entities.begin(), new_entities.end());
            simulations_stats->addNewGeneration(entities, generation);
            startAllEntitiesThread();
            pauseAllEntities(false);
            last_change_speed = std::chrono::high_resolution_clock::now();
        }
    }

    global_mutex.lock();
    if(game_menu) game_menu->draw(entities, generation, is_game_paused);
    global_mutex.unlock();
    SDL_RenderPresent(renderer);
}

bool Graphics::multipleTypesAreAlive() {
    // Renvoie s'il reste plusieurs espèces/types d'entitées en vie
    std::map<std::string, int> number_entity_type = getNumberEntitiesPerTypes();
    int number_type_alive = 0;
    for (auto pair : number_entity_type) {
        if (pair.second != 0) number_type_alive++;
    }
    return number_type_alive > 1;
}


void Graphics::deleteDeadEntities() {
    for(Entity* e : entities_to_delete){
        if (e == game_menu->getSelectedEntity()) {
            game_menu->setSelectedEntity(nullptr);
        }
        deleteEntity(e);
    }
    entities_to_delete.clear();
}

std::map<std::string, int> Graphics::getNumberEntitiesPerTypes() {
    //Compte le nombre d'entité en vie par type/espèce
    std::lock_guard<std::mutex> lock(global_mutex);
    std::map<std::string, int> result = {{"Guerrier", 0}, {"Archer", 0}, {"Mage", 0}, {"Golem", 0}};
    for (Entity* e : entities) {
        result[e->getType()] += 1;
    }
    return result;
}

void Graphics::handleEvent(bool* running, bool* keep_playing){
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            game_menu->setSimulationStopped(true);
            *keep_playing = false;
        }

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_RIGHT:
                    if (!is_game_paused) changeGameSpeed();
                    game_menu->faster();
                    break;
                case SDL_SCANCODE_LEFT:
                    if (!is_game_paused) changeGameSpeed();
                    game_menu->lower();
                    break;

                case SDL_SCANCODE_RETURN:
                    if (end_of_game) {
                        *running = false;
                        return;
                    }
                    break;

                case SDL_SCANCODE_ESCAPE:
                case SDL_SCANCODE_SPACE:
                    is_game_paused = !is_game_paused;
                    if (is_game_paused) {
                        changeGameSpeed();
                        stopAllEntitiesThread();
                        std::lock_guard<std::mutex> lock(global_mutex);
                        if(game_menu) game_menu->draw(entities, generation, is_game_paused);
                        SDL_RenderPresent(renderer);
                    } else {
                        startAllEntitiesThread();
                        last_change_speed = std::chrono::high_resolution_clock::now();
                    }
                    break;
                default: break;
            }
        }

        if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x = event.button.x;
            int y = event.button.y;

            if (event.button.button == SDL_BUTTON_LEFT) {

                //Click on a button ?
                std::vector<Button*> buttons = game_menu->getButtons();
                for (Button* b : buttons) {
                    SDL_Rect rect = b->getRect();
                    if ((rect.x <= x && x <= rect.x + rect.w) && (rect.y <= y && y <= rect.y + rect.h)) {
                        b->onClick(b);
                    }
                }

                //Click on an entity ?
                std::lock_guard<std::mutex> lock(global_mutex);
                Entity* selected_entity = getEntityAtPos(x, y);
                if (selected_entity == nullptr || selected_entity == game_menu->getSelectedEntity()) {
                    game_menu->setSelectedEntity(nullptr);
                } else {
                    game_menu->setSelectedEntity(selected_entity);
                }
            }
        }

        if (event.type == SDL_MOUSEMOTION) {
            int x = event.motion.x;
            int y = event.motion.y;

            //Hover a button = change color ?
            std::vector<Button*> buttons = game_menu->getButtons();
            for (Button* b : buttons) {
                SDL_Rect rect = b->getRect();
                if ((rect.x <= x && x <= rect.x + rect.w) && (rect.y <= y && y <= rect.y + rect.h)) {
                    b->setHovering(true);
                } else {
                    b->setHovering(false);
                }
            }
        }

        // Détection du redimensionnement
        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            int w = event.window.data1;
            int h = event.window.data2;

            // Si la largeur ou la hauteur est trop petite
            if (w < 490 || h < 450) {
                // On force la fenêtre à revenir à la taille minimale
                // std::max prend le plus grand des deux nombres
                SDL_SetWindowSize(window, std::max(w, 490), std::max(h, 450));
            }
        }
    }
}

Entity* Graphics::getEntityAtPos(float x, float y) {
    for (Entity* e : entities) {
        int s = e->getSize() * 64; // Taille d'affichage du sprite
        int w = s * e->getRatioHitboxWidth();   // largeur de la hitbox
        int h = s * e->getRatioHitboxHeight();   // hauteur de la hitbox

        float start_x = e->getX() - w / 2;
        float start_y = e->getY() - h / 2;
        float end_x = start_x + w;
        float end_y = start_y + h;

        if (start_x <= x && x <= end_x) {
            if (start_y <= y && y <= end_y) {
                return e;
            }
        }
    }
    return nullptr;
}

Entity* Graphics::createNewEntityFromParents(Entity* e1, Entity* e2){
    //Soigne les parents pour pas qu'ils meurent trop rapidement à la prochaine manche, au cas où ils ont déjà perdu pas mal de points de vie
    e1->setHp(e1->getMaxHp());
    e2->setHp(e2->getMaxHp());

    Entity* new_entity = instantiateChildByType(e1, e2);

    //Position
    int width, height;
    SDL_GetWindowSize(getWindow(), &width, &height);
    int x = std::rand() % (width-50) + 50; // entre 50 et width-50
    int y = std::rand() % (height-50) + 50; // entre 50 et height-50
    new_entity->setX(x);
    new_entity->setY(y);

    //HP
    int new_max_hp = calculateNewAttribute(e1->getMaxHp(), e2->getMaxHp());
    new_entity->setHp(new_max_hp);
    new_entity->setMaxHp(new_max_hp);

    //Size
    float new_size = calculateNewAttribute(e1->getSize(), e2->getSize());
    new_entity->setSize(new_size);

    //Speed
    float new_speed = calculateNewAttribute(e1->getMoveSpeed(), e2->getMoveSpeed());
    new_entity->setMoveSpeed(new_speed);

    //Damage
    int new_damage = calculateNewAttribute(e1->getWeapon()->getDamage(), e2->getWeapon()->getDamage());
    new_entity->getWeapon()->setDamage(new_damage);
    new_entity->updateAttackCooldown(); //Cooldown dépend de damage

    //Range
    int new_range = calculateNewAttribute(e1->getWeapon()->getRange(), e2->getWeapon()->getRange());
    new_entity->getWeapon()->setRange(new_range);

    return new_entity;
}

Entity* Graphics::instantiateChildByType(Entity* e1, Entity* e2){

    Entity* new_entity = nullptr;

    Entity* e_temp = nullptr;
    //Choose the type of entity (e1 or e2)
    (std::rand() % 2) ? e_temp = e1: e_temp = e2;

    std::string entity_type = e_temp->getType();

    std::vector<std::string> list_types = {"Guerrier", "Archer", "Mage", "Golem"};

    if ((std::rand() % 100) < mutationTypeRate) { // 15% de chance de muter
        //On enlève les deux types des parents pour que la mutation se fasse sur un type qui n'est pas celui des parents
        std::erase(list_types, e1->getType());
        std::erase(list_types, e2->getType());

        if (std::rand() % 2 == 0){
            entity_type = list_types[0];
        } else{
            entity_type = list_types[1];
        }
    }

    if(entity_type == "Guerrier") new_entity = new Guerrier(0, 0, renderer);
    else if(entity_type == "Archer") new_entity = new Archer(0, 0, renderer);
    else if(entity_type == "Mage") new_entity = new Mage(0, 0, renderer);
    else if(entity_type == "Golem") new_entity = new Golem(0, 0, renderer);
    else new_entity = new Guerrier(0, 0, renderer);

    return new_entity;
}

float Graphics::calculateNewAttribute(float value1, float value2){
    float X = (std::rand() % 101) / 100.0f;
    float new_value = (value1 * X) + (value2 * (1.0f - X));

    if ((std::rand() % 100) < mutationStatsRate) { // 15% de chance de muter
        float variation = (std::rand() % 21) / 100.0f; // Donne un nombre entre 0% ou 20%
        if (std::rand() % 2 == 0) variation = -variation; // Si on a 0, on transforme 0.2 en -0.2 ; si on a 1, on fait rien
        new_value *= (1.0f + variation);
    }

    return new_value;
}

void Graphics::deleteEntity(Entity* entity){
    if(entity) entity->stopThread();
    std::lock_guard<std::mutex> lock(global_mutex);
    for (auto it = entities.begin(); it != entities.end(); it++) {
        if (*it == entity) {
            Entity* ptr_to_delete = *it;
            entities.erase(it);
            delete ptr_to_delete;
            return;
        }
    }
}

void Graphics::deleteAllProjectiles(){
    for (Projectile* p : projectiles) {
        delete p;
    }

    projectiles.clear();
}

void Graphics::increaseAllEntitiesAge() {
    for (Entity* e : entities) {
        e->increaseAge();
    }
}