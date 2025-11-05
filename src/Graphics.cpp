#include "Graphics.h"
#include <iostream>

Graphics::Graphics() {
    SDL_CreateWindowAndRenderer(640, 480, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Clash of Colosseum");
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

    SDL_Quit();
}

void Graphics::setEntities(const std::vector<std::unique_ptr<Entity>>& ents) {
    entities.clear();
    for (auto& e : ents) {
        entities.push_back(e.get());
    }
}

void Graphics::update(bool* running) {
    SDL_Event event;

    SDL_SetRenderDrawColor(renderer, 230, 198, 34, 255);
    SDL_RenderClear(renderer);

    //Move and attack entities
    for(Entity* e : entities){
        Entity* closest = e->findClosestEntity(entities);
        if(closest != nullptr){
            if(e->canAttack(closest)){
                if(e->getWeapon()->type() == "Bow"){
                    e->getWeapon()->attack(closest, &projectiles, e->getX(), e->getY());
                } else{

                    e->getWeapon()->attack(closest);
                }

                e->setLastAttack(SDL_GetTicks());

                if(closest->getHp() == 0 ){
                    deleteEntity(closest);
                }
            } else{
                e->moveInDirection(closest->getX(), closest->getY());
            }
        }
        e->drawHealthBar(renderer);

        if (e->getWeapon()) {
            e->getWeapon()->draw(e->getX() + e->getSize(), e->getY(), renderer);
        }

        filledCircleRGBA(renderer, (short)e->getX(), (short)e->getY(),
                         (short)e->getSize(), 255, 0, 0, 255);
    }

    //Projectiles
    for(Projectile* p : projectiles){
        p->move();
        p->draw(renderer);

        for(Entity* e : entities){
            float dx = e->getX() - p->getX();
            float dy = e->getY() - p->getY();
            float distance = std::sqrt(dx * dx + dy * dy);

            float sumRadius = e->getSize() + 5;

            if(distance <= sumRadius){
                e->setHp(e->getHp() - p->getDamage()); //Attack
            }
        }
    }

    for (auto p = projectiles.begin(); p != projectiles.end(); ) {
        Projectile* proj = *p;
        proj->move();
        proj->draw(renderer);

        bool hit = false;

        for (Entity* e : entities) {
            float dx = e->getX() - proj->getX();
            float dy = e->getY() - proj->getY();
            float distance = std::sqrt(dx * dx + dy * dy);

            float sumRadius = e->getSize() + 5;

            if (distance <= sumRadius) {
                e->setHp(e->getHp() - proj->getDamage()); // inflige les dégâts
                hit = true;
                delete proj;
                p = projectiles.erase(p);
                break;
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