#include "Graphics.h"
#include <iostream>

Graphics::Graphics() {
    SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Clash of Colosseum");
    gameMusic = Mix_LoadMUS("../assets/musiqueCombat.mp3");
    if (!gameMusic) {
        std::cerr << "Erreur chargement musique jeu: " << Mix_GetError() << std::endl;
    }

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

void Graphics::update(bool* running) {
    SDL_Event event;

    SDL_SetRenderDrawColor(renderer, 230, 198, 34, 255);
    SDL_RenderClear(renderer);

    std::vector<Entity*> toDelete;

    for (Entity* e : entities) {
        Entity* closest = e->findClosestEntity(entities);
        if (!closest) continue;

        if (e->canAttackDistance(closest)) {
            if (e->canAttackTime()) {
                e->setState("attack");
                if (e->getWeapon()->type() == "Bow" || e->getWeapon()->type() == "Fireball") {
                    e->getWeapon()->attack(closest, e, &projectiles, e->getX(), e->getY());
                } else {
                    e->getWeapon()->attack(closest);
                }

                e->setLastAttack(SDL_GetTicks());

                if (closest->getHp() == 0) {
                    toDelete.push_back(closest); // ✅ suppression différée
                }
            }
        } else {
            e->setState("run");
            e->moveInDirection(closest->getX(), closest->getY());
        }

        e->draw(renderer);
        e->drawHealthBar(renderer);

        if (e->getWeapon()) {
            e->getWeapon()->draw(e->getX() + e->getSize(), e->getY(), renderer);
        }
    }

    for (Entity* d : toDelete) {
        deleteEntity(d);
    }


    for (auto p = projectiles.begin(); p != projectiles.end(); ) {
        Projectile* proj = *p;
        if(SDL_GetTicks() >= proj->getEndTime()){
            delete proj;
            p = projectiles.erase(p);
            continue;
        }
        proj->move();
        proj->draw(renderer);

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

    SDL_RenderPresent(renderer);

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *running = false;
        }

        if (event.type == SDL_KEYDOWN && !entities.empty()) {
            Entity* e = entities[0]; // on déplace seulement le premier pour test
            switch (event.key.keysym.scancode) {
                case SDL_SCANCODE_UP:    e->setY(e->getY() - 5); break;
                case SDL_SCANCODE_DOWN:  e->setY(e->getY() + 5); break;
                case SDL_SCANCODE_RIGHT: e->setX(e->getX() + 5); break;
                case SDL_SCANCODE_LEFT:  e->setX(e->getX() - 5); break;
                default: break;
            }
        }
    }
    if (entities.size() <= 5){
        std::cout << "Le combat est terminé !" << std::endl;
        *running = false;
        /*if(entities.size() == 1) *running = false;
        SDL_Delay(3000);

        for(int i = 0; i+1<entities.size(); i+= 2){

            Entity* new_entity = this->createNewEntityFromTwo(entities[i], entities[i+1]);
            entities.push_back(new_entity);

        }
        std::cout << "Le combat recommence !" << std::endl;*/
    }
}

Entity* Graphics::createNewEntityFromTwo(Entity* e1, Entity* e2){
    Entity* new_entity = nullptr;
    e1->setHp(e1->getMaxHp());
    e2->setHp(e2->getMaxHp());

    //Random position
    int width, height;
    SDL_GetWindowSize(getWindow(), &width, &height);
    int x = std::rand() % (width-50) + 50; // entre 50 et width-50
    int y = std::rand() % (height-50) + 50; // entre 50 et height-50

    Entity* e_temp = nullptr;
    //Choose the type of entity (e1 or e2)
    (std::rand() % 2) ? e_temp = e1: e_temp = e2;

    if(e_temp->getType() == "Guerrier"){
        new_entity = new Guerrier(x, y, renderer);
    } else if(e_temp->getType() == "Archer"){
        new_entity = new Archer(x, y, renderer);
    } else if(e_temp->getType() == "Mage"){
        new_entity = new Mage(x, y, renderer);
    } else if(e_temp->getType() == "Tank"){
        new_entity = new Golem(x, y, renderer);
    } else{
        new_entity = new Entity(x, y, renderer);
    }

    int hp_random = (std::rand() % e1->getHp() + e2->getHp()); //random hp between e1 and e2 hp
    new_entity->setHp(hp_random);
    new_entity->setMaxHp(hp_random);

    return new_entity;
}

void Graphics::deleteEntity(Entity* entity){
    for (auto it = entities.begin(); it != entities.end(); it++) {
        if (*it == entity) {
            delete *it;
            entities.erase(it);
            return;
        }
    }
}