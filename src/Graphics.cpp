#include "Graphics.h"
#include "Weapons/Bow.h"
#include "GameMenu.h"
#include "Entities/Guerrier.h"
#include "Entities/Archer.h"
#include "Entities/Mage.h"
#include "Entities/Golem.h"
#include <iostream>
#include <algorithm>

Graphics::Graphics() {
    SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Clash of Colosseum");
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
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

    if (renderer) {
        SDL_DestroyRenderer(renderer);
        this->renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }
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
        e->startThread(&entities, &projectiles, &game_time_speed, &same_type_peace, &global_mutex);
    }
}

void Graphics::stopAllEntitiesThread(){
    for(Entity* e : entities){
        e->stopThread();
    }
}

void Graphics::updateEntities(bool draw){

    std::lock_guard<std::mutex> lock(global_mutex);

    for (Entity* e : entities) {
        if(e->getHp() <= 0){
            entities_to_delete.push_back(e);
            continue;
        }

        e->updateAnimation();

        if(draw) e->draw(renderer, game_time_speed);
        if(draw && showHealthBars) e->drawHealthBar(renderer);

        SDL_Rect point = {int(e->getX()-5), int(e->getY()-5), 10, 10};
        SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
        SDL_RenderFillRect(renderer, &point);
    }
}

void Graphics::updateProjectiles(bool draw){

    std::lock_guard<std::mutex> lock(global_mutex);

    for (auto p = projectiles.begin(); p != projectiles.end(); ) {
        Projectile* proj = *p;
        if(SDL_GetTicks() >= proj->getEndTime()){
            delete proj;
            p = projectiles.erase(p);
            continue;
        }
        proj->move();
        if(draw) proj->draw(renderer, game_time_speed);

        bool hit = false;

        for (Entity* e : entities) {
            if(proj->getOwner() != nullptr && e != proj->getOwner()){
                //Collision
                float dx = e->getX() - proj->getX();
                float dy = e->getY() - proj->getY();
                float distance = std::sqrt(dx * dx + dy * dy);
                float sumRadius = e->getSize() + 10;

                if (distance <= sumRadius) {
                    e->setHp(e->getHp() - proj->getDamage()); // inflige les dégâts
                    hit = true;
                    delete proj;
                    p = projectiles.erase(p);
                    break;
                }
            }
        }
        if(!hit){
            p++;
        }
    }
}

void Graphics::update(bool* running) {

    handleEvent(running);
    game_time_speed = game_menu->getTimeSpeed();

    // On nettoie d'abord l'écran (noir par défaut)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // On dessine l'image de fond. Le NULL, NULL signifie "toute l'image" sur "toute la fenêtre"
    if (backgroundTexture) {
        SDL_RenderCopy(renderer, backgroundTexture, NULL, NULL);
    } else {
        // Si l'image n'a pas chargé, on garde ton ancienne couleur jaune 'sable' en fallback
        SDL_SetRenderDrawColor(renderer, 230, 198, 34, 255);
        SDL_RenderClear(renderer);
    }


    for(int i = 0; i<game_time_speed; i++) { // On répète l'action plusieurs fois si le temps est plus rapide
        bool draw = i == game_time_speed-1; //On dessine que si c'est la dernière update de la boucle

        updateEntities(draw);
        updateProjectiles(draw);

        if (!entities_to_delete.empty()) {
            for(Entity* e : entities_to_delete){
                deleteEntity(e);
            }
            entities_to_delete.clear();
        }

        if (entities.size() <= 5){
            stopAllEntitiesThread();
            std::lock_guard<std::mutex> lock(global_mutex);
            deleteAllProjectiles();
            generation++;
            std::vector<Entity*> new_entities; //Temporary vector

            for(int i = 0; i+1<entities.size(); i+= 2){
                //We have now 2 parents
                int number_of_children = std::rand() % 5 + 1;
                //The 2 parents make between 1 and 5 children
                for(int j = 0; j<number_of_children; j++){
                    Entity* new_entity = this->createNewEntityFromParents(entities[i], entities[i+1]);
                    new_entities.push_back(new_entity);
                }
            }
            entities.insert(entities.end(), new_entities.begin(), new_entities.end());
            startAllEntitiesThread();
        }
    }

    std::lock_guard<std::mutex> lock(global_mutex);

    if(game_menu) game_menu->draw(entities, generation);
    SDL_RenderPresent(renderer);
}

void Graphics::handleEvent(bool* running){
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_RIGHT:
                    game_menu->faster();
                    break;
                case SDL_SCANCODE_LEFT:  game_menu->lower(); break;
                default: break;
            }
        }
    }
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
    int new_size = calculateNewAttribute(e1->getSize(), e2->getSize());
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

    std::vector<std::string> list_types = {"Guerrier", "Archer", "Mage", "Tank"};

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
    else if(entity_type == "Tank") new_entity = new Golem(0, 0, renderer);
    else new_entity = new Entity(0, 0, renderer);

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
        p = nullptr;
    }

    projectiles.clear();
}